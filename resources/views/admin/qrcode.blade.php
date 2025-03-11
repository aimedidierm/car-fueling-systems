{{--
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>QR Code</title>
</head>

<body>
    <h1>Your QR Code:</h1>
    {!! $qrCode !!}
</body>

</html> --}}

@extends('layouts.layout')

@section('content')
<x-admin-nav />
<div class="p-4 sm:ml-64">
    <div class="p-4 border-2 border-gray-200 border-dashed rounded-lg dark:border-gray-700">
        <div class="flex justify-between">
            <a href="#" class="flex items-center pl-2.5 mb-5">
                <span class="self-center text-xl font-semibold whitespace-nowrap">Your generated QR Code:</span>
            </a>

        </div>
        {!! $qrCode !!}
    </div>
</div>
@stop