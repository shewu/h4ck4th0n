package map;

import java.awt.Color;

public class Spawn extends MapObject {

	public int id;
	public float xmin, xmax, ymin, ymax, mass, rad, hrat;
	public Color color;
	
	public Spawn() {}
	
	public Spawn(int id, float xmin, float xmax, float ymin, float ymax, float mass, float rad, float hrat, Color color) {
		this.id = id;
		this.xmin = xmin;
		this.xmax = xmax;
		this.ymin = ymin;
		this.ymax = ymax;
		this.mass = mass;
		this.rad = rad;
		this.hrat = hrat;
		this.color = color;
	}
	
	@Override
	public String getData() {
		return id+" "+xmin+" "+xmax+" "+ymin+" "+ymax+" "+mass+" "+rad+" "+hrat+" "+color.getRed()+" "+color.getGreen()+" "+color.getBlue();
	}
	
	@Override
	public String getName() {
		return "spawn";
	}

	@Override
	public MapObjectType getType() {
		return MapObjectType.SPAWN;
	}

	@Override
	public float getDistance(float x, float y) {
		float d = 0;
		if(x > xmax) d += x-xmax;
		else if(x < xmin) d += xmin - x;
		if(y > ymax) d += y-ymax;
		else if(y < ymin) d += ymin - y;
		return d;
	}
	
	public int getWallMode(float x, float y) {
		if(x < xmin) {
			if(y < ymin) {
				return 0;
			} else if(y > ymin) {
				return 1;
			}
		} else if(x > xmax) {
			if(y < ymin) {
				return 2;
			} else if(y > ymin) {
				return 3;
			}
		} else return 4;
		return -1;
	}
	
}
