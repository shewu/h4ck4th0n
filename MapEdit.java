import javax.swing.*;
import java.awt.*;
import java.awt.Toolkit;
import java.awt.event.*;
import java.util.*;
import java.io.*;

/*
 * Holy Balls: The Game
 * The Official Map Editor
 * Version 0.6
 */

public class MapEdit
{
	public static int SCALE;
	public static void main(String args[])
	{
		// make sure that window is not off the screen
		Toolkit toolkit = java.awt.Toolkit.getDefaultToolkit();
		Dimension screensize = toolkit.getScreenSize();
		int SWIDTH = screensize.width;
		int SHEIGHT = screensize.height;
		SCALE = (SHEIGHT-30)/MapEditPanel.MHEIGHT;
		// linux title bars take up space, so we need an offset
		JFrame frame = new JFrame("Holy Balls: The Game Map Editor");
		frame.setSize(MapEditPanel.MWIDTH*SCALE, MapEditPanel.MHEIGHT*SCALE+30);
		frame.setResizable(false);

		MapEditPanel p = new MapEditPanel();
		frame.getContentPane().add(p);

		frame.setVisible(true);
	}
}

// based on a 60x60 map

class MapEditPanel extends JPanel implements KeyListener
{
	public ArrayList<Segment> walls, sticky;
	public static final int MWIDTH = 60, MHEIGHT = 60;
	public Point here;
	public boolean drawing;
	public boolean positive;
	public boolean wall;
	public Point start, end;

	public MapEditPanel()
	{
		walls = new ArrayList<Segment>();
		sticky = new ArrayList<Segment>();		
		here = new Point(0,0);
		drawing = false;
		positive = true;
		wall = true;
		setFocusable(true);
		addKeyListener(this);
	}

	public void paintComponent(Graphics g)
	{
		super.paintComponent(g);

		// draw dots
		final int DOT_R = 2;
		g.setColor(Color.BLACK);
		for(int i = 0; i <= MHEIGHT; ++i)
		{
			for(int j = 0; j <= MWIDTH; ++j)
			{
				g.fillOval(MapEdit.SCALE*j - DOT_R, MapEdit.SCALE*i - DOT_R, 2*DOT_R, 2*DOT_R);
			}
		}

		// draw where we are
		final int HERE_R = 3;
		g.setColor(Color.RED);
		g.drawOval(MapEdit.SCALE*here.x - HERE_R, MapEdit.SCALE*here.y - HERE_R, 2*HERE_R, 2*HERE_R);

		//draw lines
		for (int i = 0; i < walls.size(); i++) {
			walls.get(i).draw(g);		
		}
		g.setColor(Color.GREEN);
		for (int i = 0; i < sticky.size(); i++) {
			sticky.get(i).draw(g);
		}
		System.out.println("Drawing? " + drawing);
		if (drawing && positive && wall) {
			System.out.println("Current line: " + start + " " + end);
			g.setColor(Color.RED);
			g.drawLine(MapEdit.SCALE*start.x, MapEdit.SCALE*start.y, MapEdit.SCALE*end.x, MapEdit.SCALE*end.y);
		} else if (drawing && positive && !wall) {
			System.out.println("Current line: " + start + " " + end);
			g.setColor(Color.GREEN);
			g.drawLine(MapEdit.SCALE*start.x, MapEdit.SCALE*start.y, MapEdit.SCALE*end.x, MapEdit.SCALE*end.y);
		} else if (drawing) {
			g.setColor(Color.BLACK);
			g.drawLine(MapEdit.SCALE*start.x, MapEdit.SCALE*start.y, MapEdit.SCALE*end.x, MapEdit.SCALE*end.y);
		}
	}

	public void keyTyped(KeyEvent e)
	{
		switch(e.getKeyChar())
		{
			case 'a':
				if(here.x > 0)
				{
					--here.x;
				}
				break;
			case 'd':
				if(here.x < WIDTH)
				{
					++here.x;
				}
				break;
			case 'w':
				if(here.y > 0)
				{
					--here.y;
				}
				break;
			case 's':
				if(here.y < HEIGHT)
				{
					++here.y;
				}
				break;
			case ' ':
				if (!drawing) {
					System.out.println("Setting start to " + here);
					start = new Point(here);
					drawing = true;
				} else {
					drawing = false;
					if (wall && positive) {
						walls.add(new Segment(start.x, start.y, end.x, end.y));
					} else if (positive) {
						sticky.add(new Segment(start.x, start.y, end.x, end.y));
					} else {
						Segment seg = new Segment(start.x, start.y, end.x, end.y);
						for (int i = 0; i < walls.size(); i++) {
							if (walls.get(i).equals(seg)) {
								walls.remove(i);
								i--;	
							}
						}
						for (int i = 0; i < sticky.size(); i++) {
							if (sticky.get(i).equals(seg)) {
								sticky.remove(i);
								i--;	
							}
						}
					}
				}
				break;
			case 'p':
				writeToFile();
				break;
			case 'k':
				wall = !wall;
				break;
			case 'x':
				positive = !positive;
				break;
			default:
				break;
		}
		if (drawing) {
			end = new Point(here);
		}
		repaint();
	}
	public void keyPressed(KeyEvent e) {}
	public void keyReleased(KeyEvent e) {}
	
	public void writeToFile()
	{
		try {
			BufferedWriter outfile = new BufferedWriter(new FileWriter("output"));

	  	for (int i = 0; i < walls.size(); i++) {
				Segment seg = walls.get(i);
				outfile.write("obs " + (seg.p1.x - MWIDTH/2) + " " + (seg.p1.y - MHEIGHT/2) + " "
											 + (seg.p2.x - MWIDTH/2)  + " " + (seg.p2.y - MHEIGHT/2) + "\n");
			}	

	  	for (int i = 0; i < sticky.size(); i++) {
				Segment seg = sticky.get(i);
				outfile.write("sobs " + (seg.p1.x - MWIDTH/2) + " " + (seg.p1.y - MHEIGHT/2) + " "
											 + (seg.p2.x - MWIDTH/2) + " " + (seg.p2.y - MHEIGHT/2) + "\n");
			}	
			outfile.close();
		} catch (Exception e) {
			System.out.println("Aw crap");
			System.exit(0);
		}
	}
}

class Segment
{
	public Point p1, p2;

	public Segment(int a, int b, int c, int d)
	{
		p1 = new Point(a, b);
		p2 = new Point(c, d);
	}

	public void draw(Graphics g)
	{
		g.drawLine(MapEdit.SCALE*p1.x, MapEdit.SCALE*p1.y, MapEdit.SCALE*p2.x, MapEdit.SCALE*p2.y);
	}

	public boolean equals(Segment s)
	{
		return p1.equals(s.p1) && p2.equals(s.p2) || p2.equals(s.p1) && p1.equals(s.p2);
	}
}

