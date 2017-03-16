package com.scut.ofo;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import android.os.Bundle;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity extends Activity implements OnClickListener{
	
	private static String TAG = "ofoocr";
	private Button btnProc;
	private ImageView imageview;
	private Bitmap bmp;
	private TextView tv;
	
	//OpenCV类库加载并初始化成功后的回调函数，在此我们不进行任何操作
	static {
		Log.i(TAG, "TryloadLibrary");
		System.loadLibrary("ofo");
		Log.i(TAG, "loadLibrary");
	}                	

	
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        imageview = (ImageView) findViewById(R.id.image_view);
        bmp = BitmapFactory.decodeResource(getResources(), R.drawable.ofo);
        imageview.setImageBitmap(bmp);
        
        tv = (TextView) findViewById(R.id.text_view);
        

        btnProc = (Button) findViewById(R.id.btn_gray_process);
        btnProc.setOnClickListener(new Button.OnClickListener()
        {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				
				int w = bmp.getWidth();
				int h = bmp.getHeight();
				
				//bmp --> rgb_pixels_data
				int[] pixels = new int[w*h];
				bmp.getPixels(pixels, 0, w, 0, 0, w, h);
				
				//rgb_pixels_data --> gray_pixels_data
				int[] feature2 = ofo.grayProc(pixels, w, h,270);
				String resultOfoOcr = "";
				for (int i=0;i<7;i++)
				{
					Log.i(TAG, "return: "+ feature2[i]);
					resultOfoOcr += Integer.toString(feature2[i]);
				}
				Log.i(TAG, "return: "+ resultOfoOcr);
				tv.setText(resultOfoOcr);
				
				

				//找了很久资源都找不到Mat转jintarray，只能用文件交互法
				String filepatha = "/sdcard/ofo_ocr/temp.jpg";
				File file = new File(filepatha);
				if (file.exists()) {
                    Bitmap bm = BitmapFactory.decodeFile(filepatha);
    				imageview.setImageBitmap(bm);
				}
				Log.i(TAG, "setImageBitmap");
				
			}
        });
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }


	@Override
	public void onClick(View arg0) {
		// TODO Auto-generated method stub
		
	}
    
}
