package map;

import java.awt.Color;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashSet;
import java.util.Scanner;
import java.util.Set;

public class Map {
	public int xmin, xmax, ymin, ymax;
	public Set<MapObject> objects;
	
	public Map(int xmin, int xmax, int ymin, int ymax) {
		this.xmin = xmin;
		this.xmax = xmax;
		this.ymin = ymin;
		this.ymax = ymax;
		objects = new HashSet<MapObject>();
	}
	
	public Map() {
		this(-30, 30, -30, 30);
	}
	
	public static Map openMap(File file) throws FileNotFoundException {
		Scanner scanner = new Scanner(file);
		
		Map map = new Map();
		
		/*
		map.minX = scanner.nextInt();
		map.maxX = scanner.nextInt();
		map.minY = scanner.nextInt();
		map.maxY = scanner.nextInt();
		*/
		
		while(scanner.hasNext()) {
			String cmd = scanner.next();
			if(cmd.equals("obs")) {
				map.objects.add(new Wall(false, scanner.nextFloat(), scanner.nextFloat(), scanner.nextFloat(), scanner.nextFloat()));
			} else if(cmd.equals("obs0")) {
				map.objects.add(new Goal(0, scanner.nextFloat(), scanner.nextFloat(), scanner.nextFloat(), scanner.nextFloat()));
			} else if(cmd.equals("obs1")) {
				map.objects.add(new Goal(1, scanner.nextFloat(), scanner.nextFloat(), scanner.nextFloat(), scanner.nextFloat()));
			} else if(cmd.equals("sobs")) {
				map.objects.add(new Wall(true, scanner.nextFloat(), scanner.nextFloat(), scanner.nextFloat(), scanner.nextFloat()));	
			} else if(cmd.equals("spawn")) {
				Spawn spawn = new Spawn();
				spawn.id = scanner.nextInt();
				spawn.xmin = scanner.nextFloat();
				spawn.xmax = scanner.nextFloat();
				spawn.ymin = scanner.nextFloat();
				spawn.ymax = scanner.nextFloat();
				spawn.mass = scanner.nextFloat();
				spawn.rad = scanner.nextFloat();
				spawn.hrat = scanner.nextFloat();
				spawn.color = new Color(scanner.nextInt(), scanner.nextInt(), scanner.nextInt());
				map.objects.add(spawn);
			} else if(cmd.equals("obj")) {
				map.objects.add(new Entity(scanner.nextInt()));
			}
		}
		
		return map;
	}
	
	public void writeMap(File file) throws IOException {
		BufferedWriter writer = new BufferedWriter(new FileWriter(file));
		
		/*
		writer.append(String.valueOf(minX)).append(' ');
		writer.append(String.valueOf(maxX)).append(' ');
		writer.append(String.valueOf(minY)).append(' ');
		writer.append(String.valueOf(maxY)).append('\n');
		*/
		
		for(MapObject obj : objects) {
			writer.append(obj.getName()).append(' ').append(obj.getData()).append('\n');
		}
	}
}
