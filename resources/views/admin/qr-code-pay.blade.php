@extends('layouts.layout')

@section('content')
<div style="padding: 16px; margin-left: 16px;">
    <div
        style="padding: 16px; border: 2px dashed #e5e7eb; border-radius: 8px; border-color: #e5e7eb; background-color: #f9fafb;">
        <div style="display: flex; justify-content: space-between;">

            <x-message-component />

            <div style="position: relative; width: 100%; max-width: 640px; max-height: 100%;">
                <div
                    style="position: relative; background-color: #ffffff; border-radius: 8px; box-shadow: 0 2px 15px rgba(0, 0, 0, 0.1);">
                    <div
                        style="display: flex; align-items: flex-start; justify-content: space-between; padding: 16px; border-bottom: 1px solid #e5e7eb; border-top-left-radius: 8px; border-top-right-radius: 8px;">
                        <h3 style="font-size: 1.25rem; font-weight: 600; color: #111827;">
                            Enter the following details
                        </h3>
                    </div>
                    <div style="padding: 24px; display: flex; flex-direction: column; gap: 24px;">

                        <form action="/hardware" method="POST" enctype="multipart/form-data">
                            @csrf

                            <input value="{{$card}}" name="card" hidden>
                            <div style="margin-bottom: 24px;">
                                <label for="amount"
                                    style="display: block; margin-bottom: 8px; font-size: 0.875rem; font-weight: 500; color: #111827;">
                                    Amount
                                </label>
                                <input type="number" id="amount" name="amount"
                                    style="background-color: #f9fafb; border: 1px solid #d1d5db; color: #111827; font-size: 0.875rem; border-radius: 8px; padding: 10px 16px; width: 100%; outline: none; transition: all 0.3s; box-sizing: border-box;">
                            </div>
                            <br>
                            <button type="submit"
                                style="background-color: #2563eb; color: white; padding: 12px 24px; font-size: 0.875rem; border-radius: 8px; text-align: center; cursor: pointer; width: 100%; border: none; transition: background-color 0.3s, box-shadow 0.3s;">
                                Confirm payment
                            </button>
                        </form>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>
@stop