<?php

namespace App\Http\Controllers;

use App\Enums\TransactionStatus;
use App\Models\FuelCode;
use App\Models\FuelFilling;
use App\Models\Transaction;
use App\Utils\ObjectFromArray;
use Illuminate\Http\Request;
use Symfony\Component\HttpFoundation\Response;
use Illuminate\Http\Response as HttpResponse;

class TransactionController extends Controller
{
    /**
     * Display a listing of the resource.
     */
    public function index()
    {
        $transactions = Transaction::latest()
            ->with('client')
            ->with('user')
            ->paginate(10);
        return view('admin.transactions', compact('transactions'));
    }

    public function webhookVerified(Request $request)
    {
        $secret = getenv('PAYPACK_WEBHOOK_SECRET') ? getenv('PAYPACK_WEBHOOK_SECRET') : '';
        $hash = base64_encode(hash_hmac('sha256', $request->getContent(), $secret, true));
        $hmacHeader = $request->header('X-Paypack-Signature');

        if (empty($hmacHeader)) {
            return response()->json([
                'message' => 'Missing webhook signature header',
                'success' => false,
            ], Response::HTTP_UNAUTHORIZED);
        }

        if ($hash !== $hmacHeader) {
            return response()->json([
                'message' => 'Invalid webhook signature',
                'success' => false,
            ], Response::HTTP_UNAUTHORIZED);
        }

        return null;
    }

    public function processPaypackWebhook(Request $request)
    {

        $verification = $this->webhookVerified($request);

        if ($verification !== null && $verification instanceof HttpResponse) {
            return $verification;
        }

        $payment = ObjectFromArray::createObject($request->all());
        $transaction = null;

        if (isset($payment->data) && isset($payment->data->ref)) {
            $transaction = Transaction::where('ref', $payment->data->ref)->first();

            if ($transaction) {
                $transaction->status = (string) $payment->data->status;

                $transaction->update();

                if ($payment->data->status === TransactionStatus::SUCCESSFUL->value) {
                    $fuelCode = FuelCode::where('transaction_id', $transaction->id)->first();

                    FuelFilling::create([
                        'client_id' => $fuelCode->client_id,
                        'liters' => $fuelCode->liters,
                        'price' => $fuelCode->price,
                        'total' => $fuelCode->total,
                        'type' => $fuelCode->type,
                        'fuel_code_id' => $fuelCode->id,
                    ]);
                }
            } else {
                logger()->error("Transaction with ref " . $payment->data->ref . " not found");
                return response()->json([
                    'success' => false,
                    'message' => 'Transaction with ref ' . $payment->data->ref . ' not found',
                    "ref" => $payment->data->ref ?? null,
                    "data" => $payment->data ?? null,
                ], Response::HTTP_NOT_FOUND);
            }
        }

        return response()->json([
            'message' => 'Webhook processed successfully',
            'success' => true,
        ], Response::HTTP_OK);
    }
}
