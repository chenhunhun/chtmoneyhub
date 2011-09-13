var Tesy = Tesy || {};
Tesy.interact = Tesy.interact || {};

Tesy.interact.getCoupons = function(){
   try{
      var stringResult = window.external.GetCoupons();
      var result = JSON.parse(stringResult);
   }catch(e){
      var result = [
         {
            id:0,
            expire:"2010-10-11",
            "large-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0L.jpg",
            "middle-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0M.jpg",
            "small-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0S.jpg"
         },
         {
            id:1,
            expire:"2010-10-11",
            "large-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0L.jpg",
            "middle-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0M.jpg",
            "small-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0S.jpg"
         },
         {
            id:1,
            expire:"2010-10-11",
            "large-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0L.jpg",
            "middle-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0M.jpg",
            "small-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0S.jpg"
         },
         {
            id:1,
            expire:"2010-10-11",
            "large-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0L.jpg",
            "middle-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0M.jpg",
            "small-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0S.jpg"
         },
         {
            id:1,
            expire:"2010-10-11",
            "large-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0L.jpg",
            "middle-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0M.jpg",
            "small-image":"file///C:/users/qbaty/AppData/Roaming/Bank/Coupons/0S.jpg"
         }
      ];
   }
   return result;
};

Tesy.interact.delCoupon = function(id){
	try{
		window.external.DeleteCoupon(Math.ceil(id));
	}catch(e){
		
	}
}

Tesy.interact.getAddress = function(){
	var adr;
	try{
		adr = window.external.GetAppData() + "Coupons/";
	}catch(e){
		adr = "images/";
	}
	
	return adr;
}


