<?php

use App\Http\Controllers\AuthController;
use App\Http\Controllers\ClientController;
use App\Http\Controllers\DashboardController;
use App\Http\Controllers\FuelFillingController;
use App\Http\Controllers\FuelPriceController;
use App\Http\Controllers\HardwareController;
use App\Http\Controllers\TransactionController;
use App\Http\Controllers\UserController;
use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| Web Routes
|--------------------------------------------------------------------------
|
| Here is where you can register web routes for your application. These
| routes are loaded by the RouteServiceProvider and all of them will
| be assigned to the "web" middleware group. Make something great!
|
*/

Route::view('/', 'auth.login')->name('login');
ROute::post('/', [AuthController::class, 'login']);
Route::get('/logout', [AuthController::class, 'logout'])->middleware('auth');
Route::get('/hardware/{card}', function ($card) {
    return view('admin.qr-code-pay', ['card' => $card]);
});
Route::post('/hardware', [HardwareController::class, 'qrCodeProcess']);

Route::group(["prefix" => "admin", "middleware" => ["auth", "adminCheck"], "as" => "admin."], function () {
    Route::get('/', [DashboardController::class, 'admin']);
    Route::resource('/clients', ClientController::class)->only('index', 'store');
    Route::put('/clients', [ClientController::class, 'update']);
    Route::resource('/fuel-fillings/price', FuelPriceController::class)->only('index', 'store');
    Route::resource('/fuel-fillings', FuelFillingController::class)->only('index');
    Route::resource('/transactions', TransactionController::class)->only('index');
    Route::put('/users', [UserController::class, 'updateSingleUser']);
    Route::view('/settings', 'admin.settings');
    Route::put('/settings', [UserController::class, 'update']);
    Route::post('/generate-qrcode', [HardwareController::class, 'generateQrCode']);
});
