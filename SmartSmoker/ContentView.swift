//
//  ContentView.swift
//  SmartSmoker
//
//  Created by Dallyn Wynnychuk on 2023-02-02.
//

import SwiftUI

struct ContentView: View {
    
    @StateObject var service = BluetoothService()
    
    var body: some View {
        VStack {
            Image(systemName: "globe")
                .imageScale(.large)
                .foregroundColor(.accentColor)
            Text("Hello, lets smoke!!")
            Text(service.peripheralStatus.rawValue).font(.title2)
            Text("\(service.AccLXValue)")
        }
        .padding()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
