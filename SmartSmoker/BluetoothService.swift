//
//  BluetoothService.swift
//  SmartSmoker
//
//  Created by Dallyn Wynnychuk on 1/8/24.
//

import Foundation
import CoreBluetooth


enum ConnectionStatus: String{
    case connected
    case disconnected
    case scanning
    case connecting
    case error
    
}

let AccLXService: CBUUID = CBUUID(string: "4fafc201-1fb5-459e-8fcc-c5c9c331914b")
let AccLXCharacteristic: CBUUID = CBUUID(string: "beb5483e-36e1-4688-b7f5-ea07361b26a8")
//        switch self {
//            case .connected:
//                return .green
//            case .disconnected:
//            return .red
//        }
//    }


class BluetoothService: NSObject, ObservableObject{
    private var centralManager: CBCentralManager!
    
    var AccLXPeripheral: CBPeripheral?
    @Published var peripheralStatus: ConnectionStatus = .disconnected
    @Published var AccLXValue: Int = 0
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func scanForPeripherals() {
        peripheralStatus = .scanning
        centralManager.scanForPeripherals(withServices: nil)
    }
}

extension BluetoothService: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            print("CB Powered On")
            scanForPeripherals()
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        
        if peripheral.name == "ESP32 Smoker" {
            print("Discovered \(peripheral.name ?? "no name")")
            AccLXPeripheral = peripheral
            centralManager.connect(peripheral)
            peripheralStatus = .connecting
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        peripheralStatus = .connected
        
        peripheral.delegate = self
        peripheral.discoverServices([AccLXService])
        centralManager.stopScan()
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        peripheralStatus = .disconnected
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        peripheralStatus = .error
        print(error?.localizedDescription ?? "No: Error:")
    }
}

extension BluetoothService: CBPeripheralDelegate{
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        for service in peripheral.services ?? [] {
            if service.uuid == AccLXService {
                print("found service for \(AccLXService)")
                peripheral.discoverCharacteristics([AccLXCharacteristic], for: service)
            }
        }
    }
    
    //notify
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        for characteristic in service.characteristics ?? [] {
            peripheral.setNotifyValue(true, for: characteristic)
            print("Found Characteristic, waiting on values")
        }
    }
    
    // repeat for all characteristics
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if characteristic.uuid == AccLXCharacteristic {
            guard let data = characteristic.value else {
                print("No data was received for \(characteristic.uuid.uuidString)")
                return
            }
            
            let sensorData: Int = data.withUnsafeBytes { $0.pointee }
            AccLXValue = sensorData
            print(sensorData)
        }
    }
}
