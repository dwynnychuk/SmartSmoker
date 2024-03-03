//
//  BluetoothService.swift
//  SmartSmoker
//
//  Created by Dallyn Wynnychuk on 1/8/24.
//

import Foundation
import CoreBluetooth


enum ConnectionStatus{
    case connected
    case disconnected
    case searching
    case connecting
    case error
    
//    var color: Color {
//        switch self {
//            case .connected:
//                return .green
//            case .disconnected:
//            return .red
//        }
//    }
}

class BluetoothService: NSObject, ObservableObject{
    private var centralManager: CBCentralManager!
    
    var AccLXPeripheral: CBPeripheral?
    @Published var peripheralStatus: ConnectionStatus = .disconnected
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func scanForPeripherals() {
        centralManager.scanForPeripherals(withServices: <#T##[CBUUID]?#>)
    }
}

extension BluetoothService: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            scanForPeripherals()
        }
    }
}
