/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.scripting;

import controlgui.com.RemoteCommand;
import controlgui.scripting.commands.Command;
import controlgui.scripting.commands.ScriptCommandException;
import controlgui.scripting.events.Event;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author kshaw
 */
public class Script {
    
    private String name;
    private List<Command> commands;
    private List events;
    private RemoteCommand remoteCommand;  // TODO: Wrap remote command in another type?
    
    public Script(String name, RemoteCommand remoteCommand){
        this.name = name;
        this.remoteCommand = remoteCommand;
        commands = new ArrayList<Command>();
        events = new ArrayList();
    }
    
    public void initFromString(String script){
        // TODO: Complete this
    }
    
    public String toString(){
        // TODO: Complete this
        return super.toString();
    }
    
    public void execute(){
        EventHandler handler = new EventHandler(remoteCommand, events);
        handler.start();
        
        Iterator<Command> commandIt = commands.iterator();
        Command command;
        while(commandIt.hasNext()){
            try {
                command = commandIt.next();
                command.execute(remoteCommand);
            } catch (ScriptCommandException ex) {
                Logger.getLogger(Script.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    public void stop(){
        // TODO: Complete this
    }
    
    public void addCommand(Command command){
        commands.add(command);
    }
    
    public void addEvent(Event event){
        events.add(event);
    }
}
