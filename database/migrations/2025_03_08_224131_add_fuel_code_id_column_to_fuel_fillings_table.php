<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    /**
     * Run the migrations.
     */
    public function up(): void
    {
        Schema::table('fuel_fillings', function (Blueprint $table) {
            $table->foreignId('fuel_code_id')->nullable()->constrained();
        });
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void
    {
        Schema::table('fuel_fillings', function (Blueprint $table) {
            $table->dropForeign(['fuel_code_id']);
            $table->dropColumn('fuel_code_id');
        });
    }
};
