package map;

public class Entity extends MapObject {

	public int spawnId;

	public Entity(int spawnId) {
		this.spawnId = spawnId;
	}

	public Entity() {}

	@Override
	public String getData() {
		return String.valueOf(spawnId);
	}

	@Override
	public String getName() {
		return "obj";
	}

	@Override
	public MapObjectType getType() {
		return MapObjectType.ENTITY;
	}

	@Override
	public float getDistance(float x, float y) {
		return Float.MAX_VALUE;
	}

}
