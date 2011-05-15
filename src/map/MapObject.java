package map;

public abstract class MapObject {
	public abstract String getName();
	public abstract String getData();
	public abstract MapObjectType getType();
	public abstract float getDistance(float x, float y);
}
