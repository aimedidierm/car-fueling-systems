<?php

namespace App\Http\Requests;

use Illuminate\Foundation\Http\FormRequest;
use Illuminate\Validation\Rule;

class ClientRequest extends FormRequest
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
        $rules = [
            'name' => 'required|string',
        ];

        if ($this->isMethod('post')) {
            $rules['card'] = ['required', 'string', 'unique:clients,card'];
            $rules['phone'] = ['required', 'string', 'regex:/^(079|078|073|072)[0-9]{7}$/', 'unique:clients,phone'];
        }

        if ($this->isMethod('put')) {
            $rules['userId'] = [
                'required',
                'exists:clients,id',
            ];

            $rules['card'] = [
                'required',
                'string',
                // Rule::unique('clients', 'card')->ignore($this->route('client')),
            ];

            $rules['phone'] = [
                'required',
                'string',
                'regex:/^(079|078|073|072)[0-9]{7}$/',
                // Rule::unique('clients', 'phone')->ignore($this->route('client')),
            ];
        }

        return $rules;
    }
}
