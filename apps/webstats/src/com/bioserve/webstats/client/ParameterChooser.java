package com.bioserve.webstats.client;

import java.util.HashMap;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.HorizontalPanel;
import com.google.gwt.user.client.ui.Hyperlink;
import com.google.gwt.user.client.ui.RootPanel;

public class ParameterChooser {
	/* Our set of choices */
	protected HashMap<String,Choice> choices = new HashMap<String, Choice>();
	protected Choice currentChoice = null;
	
	/* Variables for creating and managing the UI */ 
	private HorizontalPanel hPanel = new HorizontalPanel();
	private boolean stuffedFirstChoice = false; /* Have I stuffed the first choice yet? */
	private String choiceQuery = null;
	
	public ParameterChooser(String packHere, String choiceTitle, String choiceQuery) {
		RootPanel.get(packHere).add(hPanel);
		hPanel.add(new HTML(choiceTitle + "&nbsp;&nbsp;"));
		this.choiceQuery = choiceQuery;
	}
	
	public void setActive(String newChoice)
	{
		// If this is already the current choice, do nothing.
		if(currentChoice != null && 
				newChoice.equals(currentChoice.displayText))
		{
			return;
		}
		
		//Update the styles: deemphasize the old, emphasize new.
		if(currentChoice != null) {
			currentChoice.hyperlink.setStylePrimaryName("inactiveChoice");
		}
		Choice newCurrentChoice = choices.get(newChoice);
		if(newCurrentChoice != null)
		{
			newCurrentChoice.hyperlink.setStylePrimaryName("activeChoice");
			currentChoice = newCurrentChoice;
		}
	}
	
	public class Choice {
		protected String displayText;
		protected String queryText;
		protected Hyperlink hyperlink;
		private ParameterChooser parent = null;
		
		private void setup(String dt, String qt)
		{
			displayText = dt;
			queryText = qt;
			hyperlink = new Hyperlink(dt, choiceQuery + "=" + qt);
			hyperlink.addClickHandler(new ClickHandler() {
				public void onClick(ClickEvent event) { setActive(); }
			});
		}
		
		public Choice(String dt, String qt, ParameterChooser parent)
		{
			setup(dt, qt);
			this.parent = parent;
			parent.choices.put(dt, this);
			if(parent.stuffedFirstChoice) {
				hPanel.add(new HTML("&nbsp;|&nbsp;"));
			}
			hPanel.add(hyperlink);
			parent.stuffedFirstChoice = true;
		}
		
		public void setActive() {
			parent.setActive(displayText);
		}
	}
}
