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
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class GameListView extends ListActivity {
	private GameListAdapter adapter;
	private static List<File> currentDir;
	private MenuDrawer mDrawer;
	
    private SideMenuAdapter mAdapter;
    private ListView mList;
    private static GameListView me;
    public static native String GetConfig(String Key, String Value, String Default);
    public static native void SetConfig(String Key, String Value, String Default);
	
	private void Fill()
    {
		
		
		this.setTitle("Game List");
		List<GameListItem>fls = new ArrayList<GameListItem>();
		String Directories = GetConfig("General", "GCMPathes", "0");
		int intDirectories = Integer.parseInt(Directories);
		for (int a = 0; a < intDirectories; ++a)
		{
			String BrowseDir = GetConfig("General", "GCMPaths" + Integer.toString(a), "");
			File currentDir = new File(BrowseDir);
			File[]dirs = currentDir.listFiles();
			try
			{
				for(File ff: dirs)
				{
					if (ff.getName().charAt(0) != '.')
						if(!ff.isDirectory())
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
		}
		Collections.sort(fls);
		 
		adapter = new GameListAdapter(this,R.layout.main, fls);
		this.setListAdapter(adapter);
    }
	
	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		// TODO Auto-generated method stub
		super.onListItemClick(l, v, position, id);
		GameListItem o = adapter.getItem(position);
		if(o.getData().equalsIgnoreCase("folder")||o.getData().equalsIgnoreCase("parent directory")){
		}
		else
		{
			onFileClick(o.getPath());
		}
	}
	
	private void onFileClick(String o)
    {
    	Toast.makeText(this, "File Clicked: " + o, Toast.LENGTH_SHORT).show();
    	
        Intent intent = new Intent();
        intent.putExtra("Select", o);
        setResult(Activity.RESULT_OK, intent);
        
    	this.finish();
    }
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		super.onActivityResult(requestCode, resultCode, data);
		
		if (resultCode == Activity.RESULT_OK)
		{
			String FileName = data.getStringExtra("Select");
			Toast.makeText(this, "Folder Selected: " + FileName, Toast.LENGTH_SHORT).show();
			String Directories = GetConfig("General", "GCMPathes", "0");
			int intDirectories = Integer.parseInt(Directories);
			Directories = Integer.toString(intDirectories + 1);
			SetConfig("General", "GCMPathes", Directories);
			SetConfig("General", "GCMPaths" + Integer.toString(intDirectories), FileName);
			
			Fill();
		}
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		me = this;
		 
		mDrawer = MenuDrawer.attach(this, MenuDrawer.MENU_DRAG_CONTENT);
		 
		
		Fill();
		 
		List<SideMenuItem>dir = new ArrayList<SideMenuItem>();
		dir.add(new SideMenuItem("Browse Folder", 0));

		mList = new ListView(this);
		mAdapter = new SideMenuAdapter(this,R.layout.sidemenu,dir);
		mList.setAdapter(mAdapter);
		mList.setOnItemClickListener(mItemClickListener);
		
		mDrawer.setMenuView(mList);
	}
    private AdapterView.OnItemClickListener mItemClickListener = new AdapterView.OnItemClickListener() {
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        	SideMenuItem o = mAdapter.getItem(position);
        	
        	switch(o.getID())
        	{
        		case 0:
        			Toast.makeText(me, "Loading up the browser", Toast.LENGTH_SHORT).show();
        			Intent ListIntent = new Intent(me, FolderBrowser.class);
        			startActivityForResult(ListIntent, 1);
        		break;
        		default:
        		break;
        	}
            mDrawer.closeMenu();
        }
    };
    @Override
    public void setContentView(int layoutResID) {
        // This override is only needed when using MENU_DRAG_CONTENT.
    	mDrawer.setContentView(layoutResID);
        onContentChanged();
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
            	mDrawer.toggleMenu();
                return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onBackPressed() {
        final int drawerState = mDrawer.getDrawerState();
        if (drawerState == MenuDrawer.STATE_OPEN || drawerState == MenuDrawer.STATE_OPENING) {
        	mDrawer.closeMenu();
            return;
        }

        super.onBackPressed();
    }
}
