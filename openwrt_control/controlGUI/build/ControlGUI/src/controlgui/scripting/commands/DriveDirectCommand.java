/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.scripting.commands;

import controlgui.com.RemoteCommand;
import java.io.IOException;

/**
 *
 * @author kshaw
 */
public class DriveDirectCommand extends Command{

    public DriveDirectCommand(int commandType, int[] arguments){
        super(commandType, arguments);
    }
    
    public DriveDirectCommand(int commandType, int[] arguments, long duration){    
        super(commandType, arguments, duration);
    }
    
    public DriveDirectCommand(int commandType, int[] arguments, long duration, String commandId){
        super(commandType, arguments, duration, commandId);
    }
   
    protected void doExecute(RemoteCommand remoteCommand) throws ScriptCommandException{
        try {
            short right = (short) getArguments()[0];
            short left = (short) getArguments()[1];

            remoteCommand.driveDirect(right, left);
        } catch (IOException ex) {
            throw new ScriptCommandException(ex);
        }
    }

}
