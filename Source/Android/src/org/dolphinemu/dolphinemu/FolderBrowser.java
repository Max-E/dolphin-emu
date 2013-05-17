package org.dolphinemu.dolphinemu;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import net.simonvt.menudrawer.MenuDrawer;

import android.app.Activity;
import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class FolderBrowser extends ListActivity {
	private GameListAdapter adapter;
	private static File currentDir = null;
	private void Fill(File f)
    {
		File[]dirs = f.listFiles();
		this.setTitle("Current Dir: " + f.getName());
		List<GameListItem>dir = new ArrayList<GameListItem>();
		List<GameListItem>fls = new ArrayList<GameListItem>();
         
		try
		{
			for(File ff: dirs)
			{
				if (ff.getName().charAt(0) != '.')
					if(ff.isDirectory())
	                	dir.add(new GameListItem(getApplicationContext(), ff.getName(),"Folder",ff.getAbsolutePath()));
					else
						if (ff.getName().toLowerCase().contains(".gcm") ||
		                		ff.getName().toLowerCase().contains(".iso") ||
		                		ff.getName().toLowerCase().contains(".wbfs") ||
		                		ff.getName().toLowerCase().contains(".gcz") ||
		                		ff.getName().toLowerCase().contains(".dol") ||
		                		ff.getName().toLowerCase().contains(".elf"))
		                			fls.add(new GameListItem(getApplicationContext(), ff.getName(),"File Size: "+ff.length(),ff.getAbsolutePath()));
             }
         }
         catch(Exception e)
         {
         }
         
		Collections.sort(dir);
        Collections.sort(fls);
        dir.addAll(fls);
         if (!f.getName().equalsIgnoreCase("sdcard"))
        	 dir.add(0, new GameListItem(getApplicationContext(), "..", "Parent Directory", f.getParent()));

         adapter = new GameListAdapter(this,R.layout.folderbrowser,dir);
		 this.setListAdapter(adapter);
    }

	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		// TODO Auto-generated method stub
		super.onListItemClick(l, v, position, id);
		GameListItem o = adapter.getItem(position);
		if(o.getData().equalsIgnoreCase("folder")||o.getData().equalsIgnoreCase("parent directory")){
			currentDir = new File(o.getPath());
			Fill(currentDir);
		}
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		 
		if(currentDir == null)
			currentDir = new File(Environment.getExternalStorageDirectory().getPath());
		Fill(currentDir);
	}
    @Override
    public void onBackPressed() {
        Intent intent = new Intent();
        intent.putExtra("Select", currentDir.getPath());
        setResult(Activity.RESULT_OK, intent);
        
    	this.finish();
    	super.onBackPressed();
    }
}
