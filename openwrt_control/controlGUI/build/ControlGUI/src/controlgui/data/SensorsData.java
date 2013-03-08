/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlgui.data;

/**
 *
 * @author kshaw
 */
public class SensorsData {
    private int velocity;
    private int distance;
    private int playButton;
    private int bumpLeft;
    private int bumpRight;
    private int wall;
    
    private int cliffLeft;
    private int cliffFrontLeft;
    private int cliffFrontRight;
    private int cliffRight;
    private int chargingSourcesAvailable;

    public SensorsData(){
        
    }
    
    public SensorsData(String values){
        initialize(values);
    }
    
    public boolean initialize(String values){
        if(values == null || !values.startsWith("[")){
            return false;
        }
        
        String modValues = values.substring(1, values.length() -1);
        String[] splitValues = modValues.split(",");
        
        int index = 0;
        velocity = Integer.parseInt(splitValues[index++]);
        distance = Integer.parseInt(splitValues[index++]);
        playButton = Integer.parseInt(splitValues[index++]);
        bumpLeft = Integer.parseInt(splitValues[index++]);
        bumpRight = Integer.parseInt(splitValues[index++]);
        wall = Integer.parseInt(splitValues[index++]);
    
        cliffLeft = Integer.parseInt(splitValues[index++]);
        cliffFrontLeft = Integer.parseInt(splitValues[index++]);
        cliffFrontRight = Integer.parseInt(splitValues[index++]);
        cliffRight = Integer.parseInt(splitValues[index++]);
        chargingSourcesAvailable = Integer.parseInt(splitValues[index++]);
        
        return true;
    }
    
    public String toString(){
        StringBuffer result = new StringBuffer()
            .append("Velocity:").append(velocity).append("\r\n")
            .append("Distance:").append(distance).append("\r\n")
            .append("PlayButton:").append(playButton).append("\r\n")
            .append("BumpLeft:").append(bumpLeft).append("\r\n")
            .append("BumpRight:").append(bumpRight).append("\r\n")
            .append("Wall:").append(wall).append("\r\n")
            .append("CliffLeft:").append(cliffLeft).append("\r\n")
            .append("CliffFrontLeft:").append(cliffFrontLeft).append("\r\n")
            .append("CliffFrontRight:").append(cliffFrontRight).append("\r\n")
            .append("CliffRight:").append(cliffRight).append("\r\n")
            .append("ChargingSourcesAvailable:").append(chargingSourcesAvailable);
        
        return result.toString();
    }
    
    public int getVelocity() {
        return velocity;
    }

    public void setVelocity(int velocity) {
        this.velocity = velocity;
    }

    public int getDistance() {
        return distance;
    }

    public void setDistance(int distance) {
        this.distance = distance;
    }

    public int getPlayButton() {
        return playButton;
    }

    public void setPlayButton(int playButton) {
        this.playButton = playButton;
    }

    public int getBumpLeft() {
        return bumpLeft;
    }

    public void setBumpLeft(int bumpLeft) {
        this.bumpLeft = bumpLeft;
    }

    public int getBumpRight() {
        return bumpRight;
    }

    public void setBumpRight(int bumpRight) {
        this.bumpRight = bumpRight;
    }

    public int getWall() {
        return wall;
    }

    public void setWall(int wall) {
        this.wall = wall;
    }

    public int getCliffLeft() {
        return cliffLeft;
    }

    public void setCliffLeft(int cliffLeft) {
        this.cliffLeft = cliffLeft;
    }

    public int getCliffFrontLeft() {
        return cliffFrontLeft;
    }

    public void setCliffFrontLeft(int cliffFrontLeft) {
        this.cliffFrontLeft = cliffFrontLeft;
    }

    public int getCliffFrontRight() {
        return cliffFrontRight;
    }

    public void setCliffFrontRight(int cliffFrontRight) {
        this.cliffFrontRight = cliffFrontRight;
    }

    public int getCliffRight() {
        return cliffRight;
    }

    public void setCliffRight(int cliffRight) {
        this.cliffRight = cliffRight;
    }

    public int getChargingSourcesAvailable() {
        return chargingSourcesAvailable;
    }

    public void setChargingSourcesAvailable(int chargingSourcesAvailable) {
        this.chargingSourcesAvailable = chargingSourcesAvailable;
    }
    
    
}
