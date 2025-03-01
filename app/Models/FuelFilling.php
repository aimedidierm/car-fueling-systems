<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class FuelFilling extends Model
{
    use HasFactory;

    protected $fillable = [
        'client_id',
        'liters',
        'price',
        'total',
        'type',
    ];

    public function client()
    {
        return $this->belongsTo(Client::class);
    }
}
