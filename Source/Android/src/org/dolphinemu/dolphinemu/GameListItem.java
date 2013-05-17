package org.dolphinemu.dolphinemu;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

public class GameListItem implements Comparable<GameListItem>{
    private String name;
    private String data;
    private String path;
    private Bitmap image;
    public static native int[] GetBanner(String filename);
    public static native String GetTitle(String filename);
	static
	{
		try
		{
			System.loadLibrary("dolphin-emu-nogui"); 
		}
		catch (Exception ex)
		{
			Log.w("me", ex.toString());
		}
	}
    
    public GameListItem(Context ctx, String n,String d,String p)
    {
        name = n;
        data = d;
        path = p;
        File file = new File(path);
        if (!file.isDirectory())
        {
        	int[] Banner = GetBanner(path);
        	if (Banner[0] == 0)
        	{
        		try {
					InputStream path = ctx.getAssets().open("NoBanner.png");
					image = BitmapFactory.decodeStream(path);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
        		
        	}
        	else
        		image = Bitmap.createBitmap(Banner, 96, 32, Bitmap.Config.ARGB_8888);
        	name = GetTitle(path);
        }
    }
    
    public String getName()
    {
        return name;
    }
    
    public String getData()
    {
        return data;
    }
    
    public String getPath()
    {
        return path;
    }
    public Bitmap getImage()
    {
    	return image;
    }
    
    public int compareTo(GameListItem o) 
    {
        if(this.name != null)
            return this.name.toLowerCase().compareTo(o.getName().toLowerCase()); 
        else 
            throw new IllegalArgumentException();
    }
}

