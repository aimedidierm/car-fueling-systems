<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class Client extends Model
{
    use HasFactory;

    protected $fillable = [
        'name',
        'phone',
        'card',
    ];

    public function transactions()
    {
        return $this->hasMany(Transaction::class);
    }

    public function fuelFillings()
    {
        return $this->hasMany(FuelFilling::class);
    }
}
