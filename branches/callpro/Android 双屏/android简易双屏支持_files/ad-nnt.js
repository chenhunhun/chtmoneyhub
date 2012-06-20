/*
on blog-->add on 6.15-6.21
*/
var cloudad_urls = [
'http://ad-apac.doubleclick.net/clk;258383876;82593319;d?http://www.ntt.com/global-ict/?banner_id=b_gbc2012147'
, 'http://ad-apac.doubleclick.net/clk;258383876;82593320;v?http://www.ntt.com/global-ict/?banner_id=b_gbc2012147'
];

function cloudad_show() {
    var rd = Math.random();
    if (rd < 0.0074) {
        var ad_url = cloudad_urls[0];
        if (rd < 0.0038) ad_url = cloudad_urls[1];

        cloudad_doRequest(ad_url, false);

        var view_url = 'http://ad.csdn.net/log.ashx';
        view_url += '?t=click&adtype=nnt&adurl=' + encodeURIComponent(ad_url);
        cloudad_doRequest(view_url, false);
    }
}
function cloudad_doRequest(url, useFrm) {
    var e = document.createElement(useFrm ? "iframe" : "img");
    
    e.style.width = "1px";
    e.style.height = "1px";
    e.style.position = "absolute";
    e.style.visibility = "hidden";

    if (url.indexOf('?') > 0) url += '&r_m=';
    else url += '?r_m=';
    url += new Date().getMilliseconds();
    e.src = url;

    document.body.appendChild(e);
}

setTimeout(function () {
    cloudad_show();
}, 1000);
