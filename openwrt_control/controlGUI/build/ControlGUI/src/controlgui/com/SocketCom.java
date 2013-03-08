/*
 * SocketCom.java
 *
 * Created on October 25, 2007, 10:37 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package controlgui.com;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

/**
 *
 * @author tss
 */
public class SocketCom {
    
    public static final int PORT_NUM = 5012;
    public static final int CONNECT_TIMEOUT = 3000;
    
    private Socket comSocket;
    private DataOutputStream out;
    private BufferedReader in;
    
    /** Creates a new instance of SocketCom */
    public SocketCom(){
        
    }
    
    public SocketCom(String host) throws IOException {
        connect(host);
    }    
    
    public boolean connect(String host) throws IOException {
        comSocket = new Socket(host, PORT_NUM);
            
        out = new DataOutputStream(comSocket.getOutputStream());
        in = new BufferedReader(new InputStreamReader(comSocket.getInputStream()));
        return true;
    }
    
    public boolean isConnected() {
        if(comSocket != null && comSocket.isConnected()){
            return true;
        }
        
        return false;
    }
    
    public void close() throws IOException {
        if(comSocket != null){
            comSocket.close();
        }
        comSocket = null;
    }
    
    public void write(byte[] string, int length) throws IOException{
        out.write(string, 0, length);
    }
    
        // read() will return "" if there is nothing to be read
    public String read() throws IOException {
        char[] buf = new char[150];
        if(in.ready()){
            int count = in.read(buf);
            if(count >= 0)
                return new String(buf, 0, count);
        }
        return "";
    }
    
        // readResult() will block until input is available
    public String readResult() throws IOException {
        char[] buf = new char[150];
        int count = in.read(buf);
            if(count >= 0)
                return new String(buf, 0, count);
        return "";        
    }
    
    public void sendCommand(short command) throws IOException{
        byte[] string = new byte[1];
        string[0] = (byte) command;
        write(string, 1);
    }
    
    public void sendCommand(short command, short param1) throws IOException{
        byte[] string = new byte[3];
        string[0] = (byte) command;
        add16BitIntToByteArray(param1, string, 1);
        write(string, 3);
    }
    
    public void sendCommand(short command, short param1, short param2) throws IOException{
        byte[] string = new byte[5];
        string[0] = (byte)command;
        add16BitIntToByteArray(param1, string, 1);
        add16BitIntToByteArray(param2, string, 3);
        write(string, 5);
    }
    
    private byte[] add16BitIntToByteArray(short integer, byte[] array, int index){
        byte b1, b2;
        array[index] = (byte)(integer >> 8);
        array[index + 1] = (byte)(integer & 0xFF);
        System.out.println((short)array[index] + " " + (short)array[index+1]);
        
        return array;
    }
															
}
