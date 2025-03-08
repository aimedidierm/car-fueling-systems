<?php

namespace App\Http\Requests;

use Illuminate\Foundation\Http\FormRequest;

class HardwareRequest extends FormRequest
{
    /**
     * Determine if the user is authorized to make this request.
     */
    public function authorize(): bool
    {
        return true;
    }

    /**
     * Get the validation rules that apply to the request.
     *
     * @return array<string, \Illuminate\Contracts\Validation\ValidationRule|array|string>
     */
    public function rules(): array
    {
        return [
            'card' => 'sometimes|string|required_unless:code,null',
            'amount' => 'sometimes|numeric|required_unless:code,null',
            'code' => 'sometimes|string|required_if:card,null',
        ];
    }
}
