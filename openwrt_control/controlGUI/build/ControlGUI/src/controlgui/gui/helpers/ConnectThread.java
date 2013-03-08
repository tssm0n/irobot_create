/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.gui.helpers;

import controlgui.com.RemoteCommand;
import controlgui.common.ControlGUIProperties;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author kshaw
 */
public class ConnectThread extends Thread{

    private boolean complete = false;
    private boolean stop = false;
    
    private RemoteCommand remoteCommand;
    
    public ConnectThread(RemoteCommand remoteCommand){
        this.remoteCommand = remoteCommand;
    }

    public void run() {
        try {  
            if(remoteCommand == null){
                remoteCommand = new RemoteCommand();
            } else {     
                if(remoteCommand.isConnected()){
                    remoteCommand.close();
                }
            }
            
            remoteCommand.connect(ControlGUIProperties.getProperty("hostname"));
            
            while(!stop && !complete){
                complete = remoteCommand.isConnected();
            }
            
        } catch (IOException ex) {
                Logger.getLogger(ConnectThread.class.getName()).log(Level.SEVERE, null, ex);
                remoteCommand = null;
                complete = true;
        }
        System.out.println("Done Connecting");
    }
    
    public void forceStop(){
        stop = true;
    }

    public boolean isComplete() {
        return complete;
    }

    public RemoteCommand getRemoteCommand() {
        return remoteCommand;
    }
    
    
    
}
