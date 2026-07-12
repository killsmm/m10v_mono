function wait(){
    var time1 = new Date().getTime();
    var time2 = new Date().getTime();

    while ((time2 - time1) < 500){
     time2 = new Date().getTime();
    }
}


