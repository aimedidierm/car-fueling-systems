<?php

namespace App\Http\Controllers;

use App\Http\Requests\FuelPriceRequest;
use App\Models\FuelPrice;

class FuelPriceController extends Controller
{
    /**
     * Display a listing of the resource.
     */
    public function index()
    {
        $prices = FuelPrice::latest()->paginate(10);
        return view('admin.prices', compact('prices'));
    }

    /**
     * Store a newly created resource in storage.
     */
    public function store(FuelPriceRequest $request)
    {
        FuelPrice::create([
            'price' => $request->input('price'),
        ]);

        return redirect('/admin/fuel-fillings/price')->with('success', 'Fuel updated successfully');
    }
}
