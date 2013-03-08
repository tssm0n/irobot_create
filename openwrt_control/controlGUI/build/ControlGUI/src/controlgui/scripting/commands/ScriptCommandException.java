/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.scripting.commands;

/**
 *
 * @author kshaw
 */
public class ScriptCommandException extends Exception {
    public ScriptCommandException(String message){
        super(message);
    }
    
    public ScriptCommandException(Exception e){
        super(e);
    }
}
