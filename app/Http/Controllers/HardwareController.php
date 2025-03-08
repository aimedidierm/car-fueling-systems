<?php

namespace App\Http\Controllers;

use App\Enums\FuelFillingType;
use App\Enums\TransactionStatus;
use App\Http\Requests\HardwareRequest;
use App\Models\Client;
use App\Models\FuelCode;
use App\Models\FuelFilling;
use App\Models\FuelPrice;
use App\Models\Transaction;
use Paypack\Paypack;
use Symfony\Component\HttpFoundation\Response;

class HardwareController extends Controller
{
    /**
     * Display the specified resource.
     */
    public function show(HardwareRequest $request)
    {
        if ($request->has('card')) {
            $user = Client::where('card', $request->card)->first();

            if ($user) {

                $paypackInstance = $this->paypackConfig()->Cashin([
                    "amount" => $request->amount,
                    "phone" => $user->phone,
                ]);

                $transaction = Transaction::create([
                    'client_id' => $user->id,
                    'amount' => $request->amount,
                    'status' => TransactionStatus::PENDING->value,
                    'phone' => $user->phone,
                    'ref' => $paypackInstance['ref'],
                ]);

                $fuel = FuelPrice::latest()->first();
                $fuelVolume = $request->amount / $fuel->price;

                $randomCode = rand(100000, 999999);

                $fuelCode = FuelCode::create([
                    'client_id' => $user->id,
                    'liters' => $fuelVolume,
                    'price' => $fuel->price,
                    'total' => $request->amount,
                    'type' => FuelFillingType::CARD->value,
                    'code' => $randomCode,
                    'transaction_id' => $transaction->id,
                ]);

                return response()->json([
                    'status' => "1",
                    'message' => 'Please Pay',
                    'code' => $fuelCode->code,
                ], Response::HTTP_OK);
            } else {
                return response()->json([
                    'status' => "0",
                    'message' => 'CardNotFound',
                ], Response::HTTP_NOT_FOUND);
            }
        }

        if ($request->has('code')) {
            $fuelCode = FuelCode::where('code', $request->code)->first();

            if ($fuelCode) {

                if ($fuelCode->used) {
                    return response()->json([
                        'status' => "0",
                        'message' => 'CodeUsed',
                    ], Response::HTTP_NOT_FOUND);
                }

                $fuelFilling = FuelFilling::where('fuel_code_id', $fuelCode->id)->first();

                if ($fuelFilling) {
                    $fuelCode->used = true;
                    $fuelCode->update();

                    return response()->json([
                        'status' => "2",
                        'amount' => $fuelFilling->liters,
                    ], Response::HTTP_OK);
                } else {
                    return response()->json([
                        'status' => "0",
                        'message' => 'FuelNotPayed',
                    ], Response::HTTP_NOT_FOUND);
                }
            } else {
                return response()->json([
                    'status' => "0",
                    'message' => 'CodeNotFound',
                ], Response::HTTP_NOT_FOUND);
            }

            return response()->json([
                'status' => "0",
                'message' => 'CodeNotFound',
            ], Response::HTTP_NOT_FOUND);
        }

        return response()->json([
            'status' => "0",
            'message' => 'InvalidReq',
        ], Response::HTTP_BAD_REQUEST);
    }

    public function paypackConfig()
    {
        $paypack = new Paypack();

        $paypack->config([
            'client_id' => env('PAYPACK_CLIENT_ID'),
            'client_secret' => env('PAYPACK_CLIENT_SECRET'),
        ]);

        return $paypack;
    }
}
