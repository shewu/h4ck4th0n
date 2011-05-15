package map;

public class Wall extends MapObject {

	public boolean isSticky;
	public float x0, y0, x1, y1;
	
	public Wall() {}
	
	public Wall(boolean isSticky, float x0, float y0, float x1, float y1) {
		this.isSticky = isSticky;
		this.x0 = x0;
		this.y0 = y0;
		this.x1 = x1;
		this.y1 = y1;
	}
	
	@Override
	public String getData() {
		return x0 + " " + y0 + " " + x1 + " " + y1;
	}

	@Override
	public String getName() {
		return isSticky ? "sobs" : "obs";
	}

	@Override
	public MapObjectType getType() {
		return MapObjectType.WALL;
	}

	@Override
	public float getDistance(float x, float y) {
		float x0 = this.x0 - x;
		float y0 = this.y0 - y;
		float x1 = this.x1 - x;
		float y1 = this.y1 - y;
		float dx = this.x1 - this.x0;
		float dy = this.y1 - this.y0;
		
		float d0 = x0*dx+y0*dy;
		if(d0 < 0) {
			float d1 = x1*dx+y1*dy;
			if(d1 > 0) {//inside
				float d = d1-d0;
				float nx = (x0 * d1 - x1 * d0)/d;
				float ny = (y0 * d1 - y1 * d0)/d;
				return nx*nx+ny*ny;
			} else {//point 1
				return x1*x1 + y1*y1;
			}
		} else {//point 0
			return x0*x0+y0*y0;
		}
	}

	public int getWallMode(float x, float y) {
		float x0 = this.x0 - x;
		float y0 = this.y0 - y;
		float x1 = this.x1 - x;
		float y1 = this.y1 - y;
		float dx = this.x1 - this.x0;
		float dy = this.y1 - this.y0;
		
		float d0 = x0*dx+y0*dy;
		if(d0 < 0) {
			float d1 = x1*dx+y1*dy;
			if(d1 > 0) {//inside
				return 2;
			} else {//point 1
				return 1;
			}
		} else {//point 0
			return 0;
		}
	}
	
}
