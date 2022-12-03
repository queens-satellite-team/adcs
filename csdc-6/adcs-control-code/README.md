# adcs-control-code
The ADCS control code is currently limited to just a PID controller to spin the satllite using reaction wheels. Gyroscopes are used for position knowledge. The following functionality is planned on being implemented:

- [ ] Adding further sensors:
  - [ ] Accelerometers
  - [ ] Magnetometers
  - [ ] Sun sensors
- [ ] Add magnetorquers
- [ ] Adding new control modes:
  - [ ] Desaturation
  - [ ] Detumble
  - [ ] Maintenance  
        This mode will be to maintain a nominal attitude pointed at the earth
  - [ ] Fault/Error handling
  - [ ] State assesment
- [ ] A command handler that determines which mode the system should be in
