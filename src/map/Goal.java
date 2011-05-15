package map;

public class Goal extends Wall {

	public int team;

	public Goal() {
		isSticky = true;
	}

	public Goal(int team, float x0, float y0, float x1, float y1) {
		super(true, x0, y0, x1, y1);
		this.team = team;
	}
	
	@Override
	public String getName() {
		return "obs" + team;
	}
	
	@Override
	public MapObjectType getType() {
		return MapObjectType.GOAL;
	}

}
