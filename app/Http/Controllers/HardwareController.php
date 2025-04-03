<?php

namespace App\Http\Controllers;

use App\Enums\FuelFillingType;
use App\Enums\TransactionStatus;
use App\Http\Requests\HardwareRequest;
use App\Http\Requests\QrCodeProcessRequest;
use App\Http\Requests\QrCodeRequest;
use App\Models\Client;
use App\Models\FuelCode;
use App\Models\FuelFilling;
use App\Models\FuelPrice;
use App\Models\Transaction;
use SimpleSoftwareIO\QrCode\Facades\QrCode;
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
                $fuelVolume = ($request->amount / $fuel->price) * 1000;

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
                    'code' => "$fuelCode->code",
                ], Response::HTTP_OK);
            } else {
                return response()->json([
                    'status' => "0",
                    'message' => 'Card Not Found',
                ], Response::HTTP_NOT_FOUND);
            }
        }

        if ($request->has('code')) {
            $fuelCode = FuelCode::where('code', $request->code)->first();

            if ($fuelCode) {

                if ($fuelCode->used) {
                    return response()->json([
                        'status' => "0",
                        'message' => 'Code Used',
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
                        'message' => 'Fuel Not Payed',
                    ], Response::HTTP_NOT_FOUND);
                }
            } else {
                return response()->json([
                    'status' => "0",
                    'message' => 'Code Not Found',
                ], Response::HTTP_NOT_FOUND);
            }

            return response()->json([
                'status' => "0",
                'message' => 'Code Not Found',
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

    public function generateQrCode(QrCodeRequest $request)
    {
        $client = Client::where('phone', $request->phone)->first();
        $url = env('APP_URL') . '/hardware/' . $client->card;
        // return $url;
        $qrCode = QrCode::size(300)->generate($url);

        return view('admin.qrcode', compact('qrCode', 'client'));
    }

    public function qrCodeProcess(QrCodeProcessRequest $request)
    {
        $client = Client::where('card', $request->card)->first();
        if ($client) {
            $paypackInstance = $this->paypackConfig()->Cashin([
                "amount" => $request->amount,
                "phone" => $client->phone,
            ]);

            $transaction = Transaction::create([
                'client_id' => $client->id,
                'amount' => $request->amount,
                'status' => TransactionStatus::PENDING->value,
                'phone' => $client->phone,
                'ref' => $paypackInstance['ref'],
            ]);

            $fuel = FuelPrice::latest()->first();
            $fuelVolume = ($request->amount / $fuel->price) * 1000;

            $randomCode = rand(100000, 999999);

            FuelCode::create([
                'client_id' => $client->id,
                'liters' => $fuelVolume,
                'price' => $fuel->price,
                'total' => $request->amount,
                'type' => FuelFillingType::CARD->value,
                'code' => $randomCode,
                'transaction_id' => $transaction->id,
            ]);

            return back()->with('success', 'Code is: ' . $randomCode);
        } else {
            return back()->withErrors('User not found');
        }
    }
}
