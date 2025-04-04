@extends('layouts.layout')

@section('content')
<x-admin-nav />
<div class="p-4 sm:ml-64">
    <div class="p-4 border-2 border-gray-200 border-dashed rounded-lg dark:border-gray-700">
        <div class="flex justify-between">
            <a href="#" class="flex items-center pl-2.5 mb-5">
                <span class="self-center text-xl font-semibold whitespace-nowrap">User Dashboard</span>
            </a>
            <button type="button" data-modal-target="defaultModal" data-modal-toggle="defaultModal"
                class="text-white bg-blue-700 hover:bg-blue-800 focus:outline-none focus:ring-4 focus:ring-blue-300 font-medium rounded-full text-sm px-5 py-2.5 text-center mr-2 mb-2 dark:bg-blue-600 dark:hover:bg-blue-700 dark:focus:ring-blue-800">QR
                Code
            </button>

            <x-message-component />

            <div id="defaultModal" tabindex="-1" aria-hidden="true"
                class="fixed top-0 left-0 right-0 z-50 hidden w-full p-4 overflow-x-hidden overflow-y-auto md:inset-0 h-[calc(100%-1rem)] max-h-full">
                <div class="relative w-full max-w-2xl max-h-full">
                    <div class="relative bg-white rounded-lg shadow dark:bg-gray-700">
                        <div class="flex items-start justify-between p-4 border-b rounded-t dark:border-gray-600">
                            <h3 class="text-xl font-semibold text-gray-900 dark:text-white">
                                Generate QR Code
                            </h3>
                            <button type="button"
                                class="text-gray-400 bg-transparent hover:bg-gray-200 hover:text-gray-900 rounded-lg text-sm w-8 h-8 ml-auto inline-flex justify-center items-center dark:hover:bg-gray-600 dark:hover:text-white"
                                data-modal-hide="defaultModal">
                                <svg class="w-3 h-3" aria-hidden="true" xmlns="http://www.w3.org/2000/svg" fill="none"
                                    viewBox="0 0 14 14">
                                    <path stroke="currentColor" stroke-linecap="round" stroke-linejoin="round"
                                        stroke-width="2" d="m1 1 6 6m0 0 6 6M7 7l6-6M7 7l-6 6" />
                                </svg>
                                <span class="sr-only">Close modal</span>
                            </button>
                        </div>
                        <div class="p-6 space-y-6">

                            <form action="/admin/generate-qrcode" method="POST" enctype="multipart/form-data">
                                @csrf
                                <div class="mb-6">
                                    <label for="phone"
                                        class="block mb-2 text-sm font-medium text-gray-900 dark:text-white">
                                        Client Phone</label>
                                    <input type="number" id="phone" name="phone"
                                        class="bg-gray-50 border border-gray-300 text-gray-900 text-sm rounded-lg focus:ring-blue-500 focus:border-blue-500 block w-full p-2.5 dark:bg-gray-700 dark:border-gray-600 dark:placeholder-gray-400 dark:text-white dark:focus:ring-blue-500 dark:focus:border-blue-500"
                                        required>
                                </div>
                                <br>
                                <button type="submit"
                                    class="text-white bg-blue-700 hover:bg-blue-800 focus:ring-4 focus:outline-none focus:ring-blue-300 font-medium rounded-lg text-sm w-full sm:w-auto px-5 py-2.5 text-center dark:bg-blue-600 dark:hover:bg-blue-700 dark:focus:ring-blue-800">Generate
                                    QR Code
                                </button>
                            </form>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <div class="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-4 gap-4 mt-8">
            <div
                class="bg-white dark:bg-gray-800 border border-gray-200 rounded-lg shadow-md p-6 flex items-center justify-center">
                <div class="text-center">
                    <p class="text-xl font-semibold text-gray-900 dark:text-white">Clients</p>
                    <p class="text-3xl font-bold text-blue-600 dark:text-blue-400">{{$clients}}</p>
                </div>
            </div>
            <div
                class="bg-white dark:bg-gray-800 border border-gray-200 rounded-lg shadow-md p-6 flex items-center justify-center">
                <div class="text-center">
                    <p class="text-xl font-semibold text-gray-900 dark:text-white">Transactions</p>
                    <p class="text-3xl font-bold text-blue-600 dark:text-blue-400">{{$transactions}}</p>
                </div>
            </div>
            <div
                class="bg-white dark:bg-gray-800 border border-gray-200 rounded-lg shadow-md p-6 flex items-center justify-center">
                <div class="text-center">
                    <p class="text-xl font-semibold text-gray-900 dark:text-white">Fuel Fillings</p>
                    <p class="text-3xl font-bold text-blue-600 dark:text-blue-400">{{$fillings}}</p>
                </div>
            </div>
            <div
                class="bg-white dark:bg-gray-800 border border-gray-200 rounded-lg shadow-md p-6 flex items-center justify-center">
                <div class="text-center">
                    <p class="text-xl font-semibold text-gray-900 dark:text-white">Prices </p>
                    <p class="text-3xl font-bold text-blue-600 dark:text-blue-400">{{$prices->price}} Rwf</p>
                </div>
            </div>
        </div>
    </div>
</div>
@stop