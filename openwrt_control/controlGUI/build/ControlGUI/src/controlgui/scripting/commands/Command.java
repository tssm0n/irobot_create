/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.scripting.commands;

import controlgui.com.RemoteCommand;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author kshaw
 */
public abstract class Command {

    private String commandId;
    private int commandType;
    private int[] arguments;
    private long duration = 0; // Duration is in seconds, 0 for instant, -1 for infinite
    
    public Command(int commandType, int[] arguments){
        this.commandType = commandType;
        this.arguments = arguments;
    }
    
    public Command(int commandType, int[] arguments, long duration){    
        this(commandType, arguments);
        this.duration = duration;
    }
    
    public Command(int commandType, int[] arguments, long duration, String commandId){
        this(commandType, arguments, duration);
        this.commandId = commandId;
    }
    
    public void execute(RemoteCommand remoteCommand) throws ScriptCommandException{
        // TODO: Run this in a seperate thread so it can be interrupted?
        doExecute(remoteCommand);
        
        if(duration == -1){
            // TODO: Make this infinite, with a way to be interrupted
        } else {
            try {
                Thread.sleep(duration * 1000);
            } catch (InterruptedException ex) {
                Logger.getLogger(Command.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        
    }
    
    protected abstract void doExecute(RemoteCommand remoteCommand) throws ScriptCommandException;
    
    public void initFromString(String command){
        // TODO: Complete this
    }
    
    public String toString(){
        // TODO: Complete this
        return super.toString();
    }

    public int[] getArguments() {
        return arguments;
    }

    public void setArguments(int[] arguments) {
        this.arguments = arguments;
    }

    public String getCommandId() {
        return commandId;
    }

    public void setCommandId(String commandId) {
        this.commandId = commandId;
    }

    public int getCommandType() {
        return commandType;
    }

    public void setCommandType(int commandType) {
        this.commandType = commandType;
    }

    public long getDuration() {
        return duration;
    }

    public void setDuration(long duration) {
        this.duration = duration;
    }
    
}
