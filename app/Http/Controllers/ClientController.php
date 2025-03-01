<?php

namespace App\Http\Controllers;

use App\Http\Requests\ClientRequest;
use App\Models\Client;

class ClientController extends Controller
{
    /**
     * Display a listing of the resource.
     */
    public function index()
    {
        $clients = Client::latest()->paginate(10);
        return view('admin.clients', compact('clients'));
    }

    /**
     * Store a newly created resource in storage.
     */
    public function store(ClientRequest $request)
    {
        Client::create([
            'name' => $request->input('name'),
            'phone' => $request->input('phone'),
            'card' => $request->input('card'),
        ]);

        return redirect('/admin/clients')->with('success', 'Client added successfully');
    }

    public function update(ClientRequest $request)
    {
        $user = Client::find($request->userId);

        if ($user) {
            $user->name = $request->name;
            $user->phone = $request->phone;
            $user->card = $request->card;
            $user->update();
        } else {
            return redirect('/admin/clients')->withErrors(['msg' => 'User not found']);
        }

        return redirect('/admin/clients')->with('success', 'Client updated successfully');
    }
}
