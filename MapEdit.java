import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

public class MapEdit
{
	public static void main(String args[])
	{
		// linux title bars take up space, so we need an offset
		JFrame frame = new JFrame("Holy Balls: The Game Map Editor");
		frame.setSize(720, 720+30);
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
	public final int WIDTH = 60, HEIGHT = 60;
	public Point here;
	public boolean drawing;

	public MapEditPanel()
	{
		walls = new ArrayList<Segment>();
		sticky = new ArrayList<Segment>();		
		here = new Point(0,0);
		drawing = false;
		setFocusable(true);
		addKeyListener(this);
	}

	public void paintComponent(Graphics g)
	{
		super.paintComponent(g);

		// draw dots
		final int DOT_R = 2;
		g.setColor(Color.BLACK);
		for(int i = 0; i <= 60; ++i)
		{
			for(int j = 0; j <= 60; ++j)
			{
				g.fillOval(12*j - DOT_R, 12*i - DOT_R, 2*DOT_R, 2*DOT_R);
			}
		}

		// draw where we are
		final int HERE_R = 3;
		g.setColor(Color.RED);
		g.drawOval(12*here.x - HERE_R, 12*here.y - HERE_R, 2*HERE_R, 2*HERE_R);
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
			default:
				break;
		}
		repaint();
	}
	public void keyPressed(KeyEvent e) {}
	public void keyReleased(KeyEvent e) {}
}

class Segment
{
	public Point p1, p2;

	public Segment(int a, int b, int c, int d)
	{
		p1 = new Point(a, b);
		p2 = new Point(c, d);
	}
}
