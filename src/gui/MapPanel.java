package gui;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseMotionAdapter;

import javax.swing.JPanel;

import map.Goal;
import map.Map;
import map.MapObject;
import map.Spawn;
import map.Wall;

@SuppressWarnings("serial")
public class MapPanel extends JPanel {
	public static final Color BACKGROUND_COLOR = Color.WHITE;
	public static final Color WALL_COLOR = Color.BLACK;
	public static final Color STICKY_COLOR = Color.GREEN;
	public static final Color[] GOAL_COLOR = {Color.BLUE, Color.RED};
	public static final Color BOUNDS_COLOR = Color.BLACK;
	public static final Color TEXT_COLOR = Color.WHITE;
	
	public static final float DISTANCE = 2;

	public Map map;
	private MapEditor editor;
	MapObject edit;
	private float ratio;
	private int wallMode;
	private float lastX, lastY;
	boolean snap=false;

	public MapPanel(MapEditor editor) {
		this.editor = editor;

		super.addMouseListener(new MouseAdapter() {
			@Override
			public void mousePressed(MouseEvent e) {
				if(map != null) {
					float x = (float) (e.getX()/ratio) + map.xmin;
					float y = (float) (e.getY()/ratio) + map.ymin;
					selectObject(x, y);
				}
			}

			@Override
			public void mouseReleased(MouseEvent arg0) {
				edit = null;
			}
		});

		super.addMouseMotionListener(new MouseMotionAdapter() {

			@Override
			public void mouseDragged(MouseEvent e) {
				float x = (float) (e.getX()/ratio) + map.xmin;
				float y = (float) (e.getY()/ratio) + map.ymin;
				dragObject(x, y);
			}
		});

		super.addKeyListener(new KeyAdapter() {
			@Override
			public void keyPressed(KeyEvent e) {
				if(e.getKeyCode() == KeyEvent.VK_SHIFT) {
					snap = true;
				}
			}

			@Override
			public void keyReleased(KeyEvent e) {
				if(e.getKeyCode() == KeyEvent.VK_SHIFT) {
					snap = false;
				}
			}
		});
	}

	private void selectObject(float x, float y) {
		float distance = DISTANCE;
		MapObject closest = null;
		for(MapObject obj : map.objects) {
			float d = obj.getDistance(x, y);
			if(d < distance) {
				distance = d;
				closest = obj;
			}
		}
		if(closest != null) {
			lastX = x;
			lastY = y;

			if(closest instanceof Wall) {
				Wall wall = (Wall)closest;
				wallMode = wall.getWallMode(x, y);
			} else if(closest instanceof Spawn) {
				Spawn spawn = (Spawn)closest;
				wallMode = spawn.getWallMode(x, y);
			}

			edit = closest;
			editor.edit(edit);
		}
	}

	private void dragObject(float x, float y) {
		if(edit != null) {
			if(snap) {
				x = Math.round(x);
				y = Math.round(y);
				lastX = Math.round(lastX);
				lastY = Math.round(lastY);
			}

			if(edit instanceof Wall) {
				Wall wall = (Wall)edit;
				switch(wallMode) {
				case 0:
					wall.x0 = x;
					wall.y0 = y;
					break;
				case 1:
					wall.x1 = x;
					wall.y1 = y;
					break;
				case 2:
					float dx = x - lastX;
					float dy = y - lastY;
					wall.x0 += dx;
					wall.y0 += dy;
					wall.x1 += dx;
					wall.y1 += dy;
					lastX = x;
					lastY = y;
					break;
				}
			} else if(edit instanceof Spawn) {
				Spawn spawn = (Spawn)edit;
				switch(wallMode) {
				case 0:
					spawn.xmin = x;
					spawn.ymin = y;
					break;
				case 1:
					spawn.xmin = x;
					spawn.ymax = y;
					break;
				case 2:
					spawn.xmax = x;
					spawn.ymin = y;
					break;
				case 3:
					spawn.xmax = x;
					spawn.ymax = y;
					break;
				case 4:
					float dx = x - lastX;
					float dy = y - lastY;
					spawn.xmin += dx;
					spawn.ymin += dy;
					spawn.xmax += dx;
					spawn.ymax += dy;
					lastX = x;
					lastY = y;
					break;
				}
			}
			repaint();
		}
	}

	@Override
	public void paint(Graphics g) {
		int x = super.getX();
		int y = super.getY();
		int w = super.getWidth();
		int h = super.getHeight();

		g.setColor(BACKGROUND_COLOR);
		g.fillRect(x, y, w, h);

		if(map != null) {
			Graphics2D g2d = (Graphics2D)g;
			g2d.translate(x, y);
			ratio = Math.min(w/(map.xmax - map.xmin), h/(map.ymax - map.ymin));
			//g2d.scale(ratio, ratio);
			//g2d.translate(-map.minX, -map.minY);

			for(MapObject obj : map.objects) {
				switch(obj.getType()) {
				case WALL:
					Wall wall = (Wall)obj;
					g2d.setColor(wall.isSticky ? STICKY_COLOR : WALL_COLOR);
					g2d.drawLine((int)(ratio*(wall.x0-map.xmin)), (int)(ratio*(wall.y0-map.ymin)),
							(int)(ratio*(wall.x1-map.xmin)), (int)(ratio*(wall.y1-map.ymin)));
					break;
				case SPAWN:
					Spawn spawn = (Spawn)obj;
					g2d.setColor(spawn.color);
					g2d.fillRect((int)(ratio*(spawn.xmin - map.xmin)), (int)(ratio*(spawn.ymin - map.ymin)),
							(int)(ratio*(spawn.xmax-spawn.xmin)), (int)(ratio*(spawn.ymax-spawn.ymin)));
					g2d.setColor(TEXT_COLOR);
					g2d.drawString(String.valueOf(spawn.id),
							ratio*((spawn.xmin + spawn.xmax)/2 - map.xmin), ratio*((spawn.ymin + spawn.ymax)/2 - map.ymin));
					break;
				case GOAL:
					Goal goal = (Goal)obj;
					g2d.setColor(GOAL_COLOR[goal.team]);
					g2d.drawLine((int)(ratio*(goal.x0-map.xmin)), (int)(ratio*(goal.y0-map.ymin)),
							(int)(ratio*(goal.x1-map.xmin)), (int)(ratio*(goal.y1-map.ymin)));
					break;
				}
			}

			g2d.setColor(BOUNDS_COLOR);
			g2d.drawLine(0, 0, 0, (int)(ratio*(map.ymax-map.ymin)));
			g2d.drawLine((int)(ratio*(map.xmax-map.xmin)), (int)(ratio*(map.ymax-map.ymin)), 0, (int)(ratio*(map.ymax-map.ymin)));
			g2d.drawLine((int)(ratio*(map.xmax-map.xmin)), (int)(ratio*(map.ymax-map.ymin)), (int)(ratio*(map.xmax-map.xmin)), 0);
			g2d.drawLine(0, 0, (int)(ratio*(map.xmax-map.xmin)), 0);

		}
	}

}
