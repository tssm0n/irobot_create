/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.scripting;

import controlgui.com.CommandConstants;
import controlgui.com.RemoteCommand;
import controlgui.scripting.commands.DriveDirectCommand;

/**
 *
 * @author kshaw
 */
public class TestScript extends Script{
    public TestScript(String name, RemoteCommand remoteCommand){
        super(name, remoteCommand);
        
        init();
    }
    
    private void init(){
        int[] forwardParams = {50,50};
        addCommand(new DriveDirectCommand(CommandConstants.DRIVEDIRECT, forwardParams, 2));
        int[] stopParams = {0,0};
        addCommand(new DriveDirectCommand(CommandConstants.DRIVEDIRECT, stopParams, 0));
        int[] backParams = {-50, -50};
        addCommand(new DriveDirectCommand(CommandConstants.DRIVEDIRECT, backParams, 2));
        addCommand(new DriveDirectCommand(CommandConstants.DRIVEDIRECT, stopParams, 0));
    }
}
