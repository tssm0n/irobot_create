/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.common;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author kshaw
 */
public class ControlGUIProperties {
    private static final String PROPERTIES_FILE = "controlgui.properties";
    private static Properties properties;
    
    public static String getProperty(String key){
        if(properties == null){
            FileInputStream fis = null;
            try {
                properties = new Properties();
                fis = new FileInputStream(PROPERTIES_FILE);
                properties.load(fis);
            } catch (IOException ex) {
                Logger.getLogger(ControlGUIProperties.class.getName()).log(Level.SEVERE, null, ex);
                return null;
            } finally {
                try {
                    fis.close();
                } catch (IOException ex) {
                    Logger.getLogger(ControlGUIProperties.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
        
        return properties.getProperty(key);
    }
}
