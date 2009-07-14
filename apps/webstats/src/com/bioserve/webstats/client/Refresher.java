package com.bioserve.webstats.client;
import java.util.LinkedList;
import java.util.List;

import com.google.gwt.user.client.Timer;

public class Refresher {
	private int msRate = 5000;
	private Timer myTimer = new Timer () {
		public void run() {
			for(Updateable i : updateList)
			{
				i.update();
			}
		}
	};
	private List<Updateable> updateList = new LinkedList<Updateable>();
	
	public Refresher() {
	}
	
	public Refresher(int msRate) {
		updateRate(msRate);
	}
	
	public void updateRate(int msRate) {
		this.msRate = msRate;
		schedule();
	}
	
	public void schedule() {
		myTimer.scheduleRepeating(msRate);
	}
	
	public void add(Updateable toAdd)
	{
		updateList.add(toAdd);
	}
}
