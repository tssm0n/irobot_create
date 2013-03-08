/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.com;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author kshaw
 */
public class RemoteCommand {
    private SocketCom socketCom;
    
    public RemoteCommand(){
        socketCom = new SocketCom();
    }    
    
    public RemoteCommand(String host) throws IOException {
        socketCom = new SocketCom(host);
    }
        
    public boolean connect(String host){
        try {
            return socketCom.connect(host);
        } catch (IOException ex){
            Logger.getLogger(RemoteCommand.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }
    
    public boolean isConnected() {
        return socketCom.isConnected();
    }
    
    public void close() throws IOException{
        if(socketCom != null){
            socketCom.close();
        }
    }
    
    public synchronized String read() throws IOException{
        return socketCom.read();
    }
    
    public synchronized String readResult() throws IOException{
        return socketCom.readResult();
    }    
    
    private synchronized void sendCommand(short command) throws IOException {
        socketCom.sendCommand(command);
    }
    
    private synchronized void sendCommand(short command, short arg1) throws IOException {
        socketCom.sendCommand(command, arg1);
    } 
    
    private synchronized void sendCommand(short command, short arg1, short arg2) throws IOException {
        socketCom.sendCommand(command, arg1, arg2);
    }     
    
    public void stop() throws IOException{
        sendCommand(CommandConstants.STOP);
    }
    
    public void drive(short velocity, short radius) throws IOException {
        sendCommand(CommandConstants.DRIVE, velocity, radius);
    }
    
    public void driveDirect(short right, short left) throws IOException {
        sendCommand(CommandConstants.DRIVEDIRECT, right, left);
    }
    
    public void changeLeds(short param1, short param2) throws IOException{
        sendCommand(CommandConstants.LED, param1, param2);        
    }
    
    public synchronized String sensors() throws IOException {
        sendCommand(CommandConstants.SENSORS);
        return readResult();
    }
    
    public void driveProgram() throws IOException{
        sendCommand(CommandConstants.DRIVE_PROGRAM);
    } 
    
    public void driveProgramCustom(short speed, short customSensors) throws IOException{
        sendCommand(CommandConstants.DRIVE_PROGRAM_WITH_PARAMS, speed, customSensors);
    }     
    
    public void followWall() throws IOException{
        sendCommand(CommandConstants.FOLLOW_WALL);
    }
    
    public void cmLedsOn() throws IOException {
        sendCommand(CommandConstants.CM_LED_ON);
    }
    
    public void cmLedsOff() throws IOException {
        sendCommand(CommandConstants.CM_LED_OFF);
    }
    
    public synchronized String irSensor() throws IOException {
        sendCommand(CommandConstants.IR_SENSOR);
        return readResult();
    }
    
    public void exitProgram() throws IOException {
        sendCommand(CommandConstants.EXIT_PROGRAM);
    }
    
}
