<?php

namespace App\Http\Controllers;

use App\Models\Client;
use App\Models\FuelFilling;
use App\Models\FuelPrice;
use App\Models\Transaction;
use App\Models\Trip;

class DashboardController extends Controller
{
    public function admin()
    {
        $clients = Client::count();
        $transactions = Transaction::count();
        $fillings = FuelFilling::count();
        $prices = FuelPrice::latest()->select('price')->first();
        return view('admin.dashboard', compact(
            'clients',
            'transactions',
            'fillings',
            'prices',
        ));
    }
}
