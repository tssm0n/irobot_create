/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.scripting;

import controlgui.com.RemoteCommand;
import controlgui.data.SensorsData;
import java.io.IOException;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author kshaw
 */
public class EventHandler extends Thread {

    private static final int DEFAULT_POLL_INTERVAL = 30;
    
    private RemoteCommand remoteCommand;
    private List events;
    private long pollInterval = DEFAULT_POLL_INTERVAL;
    private boolean stop = false;
    
    public EventHandler(RemoteCommand remoteCommand, List events){
        this.remoteCommand = remoteCommand;
        this.events = events;
    }
    
    public void run(){
        SensorsData sensorsData = new SensorsData();
        String sensorsResult;
        try {
            while(!stop){
                sensorsResult = remoteCommand.sensors();
                sensorsData.initialize(sensorsResult);
                checkForEvent(sensorsData);
                Thread.sleep(pollInterval);
            }
        } catch(IOException ex){
            Logger.getLogger(EventHandler.class.getName()).log(Level.SEVERE, null, ex);
        } catch(InterruptedException ex){
            Logger.getLogger(EventHandler.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    public void exit(){
        stop = true;
    }
    
    private void checkForEvent(SensorsData sensorsData){
        // TODO: Complete this
    }
}
