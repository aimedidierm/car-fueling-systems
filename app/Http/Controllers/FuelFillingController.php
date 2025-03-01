<?php

namespace App\Http\Controllers;

use App\Models\FuelFilling;
use Illuminate\Http\Request;

class FuelFillingController extends Controller
{
    /**
     * Display a listing of the resource.
     */
    public function index()
    {
        $fuels = FuelFilling::latest()->paginate();
        $fuels->load('client');

        return view('admin.fuels', compact('fuels'));
    }
}
