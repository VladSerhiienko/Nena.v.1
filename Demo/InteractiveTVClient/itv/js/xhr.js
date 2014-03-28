var host = 'http://localhost';

var active_menu = 'main';

function setActiveMenu(selectedItem) {
    $('#l_main').removeClass('active');
    $('#l_news').removeClass('active');
    $('#l_schedue').removeClass('active');
    
    $('#l_' + selectedItem).addClass('active');
    active_menu = selectedItem;
}

function goToMainPage() {
    setActiveMenu('main');
    
    // load slider
    $.post('xhr/slider.html', function(data) {
        $('#main').html(data);
    });
    
    $('#mfoo').css('display', 'block');
}

function goToNewsPage() {
    setActiveMenu('news');
    
    $.post('xhr/news.html', function(data) {
        $('#main').html(data);
    });
    
    $('#mfoo').css('display', 'block');
}

function goToScheduePage() {
    setActiveMenu('schedue');
    
    $.post('xhr/schedue.html', function(data){
        $('#main').html(data);
    });
    
    $('#mfoo').css('display', 'none');
}

$('#l_news').click(goToNewsPage);
$('#l_main').click(goToMainPage);
$('#l_schedue').click(goToScheduePage);


function leftArrowPressed() {
    if (active_menu === 'news')
    {
        goToMainPage();
    } else if (active_menu === 'schedue') {
        goToNewsPage();
    }
}

function rightArrowPressed() {
    if (active_menu === 'main')
    {
        goToNewsPage();
    } else if (active_menu === 'news') {
        goToScheduePage();
    }
}

function topArrowPressed() {
    if (active_menu === 'news') {
        
        // get current element
        curr = $('.newpost_selected');
        
        // get prev element
        prev = curr.prev();
        
        // if first element
        if(!prev.hasClass('newpost'))
            return false;
        
        // remove current element - selected
        curr.css('display', 'none');
        curr.removeClass('newpost_selected');
        
        // set selected previous element
        prev.css('display', 'block');
        prev.addClass('newpost_selected');
    }
    
    if (active_menu === 'main') {
        $('#carousel-example-generic').carousel('prev');
    }
    
    if (active_menu === 'schedue'){
        scheact = $('.scheact');
        prev = scheact.prev();
        
        if (!prev.hasClass('sche')) {
            return false;
        }
        
        prev.css('display', 'block');
        prev.addClass('scheact');
        scheact.removeClass('scheact');
        scheact.css('display', 'none');
    }
    
    return false;
}

function downArrowPressed() {
    if (active_menu === 'news') {
        
        // get current element
        curr = $('.newpost_selected');
        
        // get prev element
        next = curr.next();
        
        // if first element
        if(!next.hasClass('newpost'))
            return false;
        
        // remove current element - selected
        curr.css('display', 'none');
        curr.removeClass('newpost_selected');
        
        // set selected previous element
        next.css('display', 'block');
        next.addClass('newpost_selected');
    }
    
    if (active_menu === 'main') {
        $('#carousel-example-generic').carousel('next');
    }
    
    if (active_menu === 'schedue'){
        scheact = $('.scheact');
        next = scheact.next();
        
        if (!next.hasClass('sche')) {
            return false;
        }
        
        next.css('display', 'block');
        next.addClass('scheact');
        scheact.removeClass('scheact');
        scheact.css('display', 'none');
    }
    
    return false;
}

document.onkeydown = function(evt) {
    evt = evt || window.event;
    switch (evt.keyCode) {
    case 37:
        leftArrowPressed();
        break;
    case 38:
        topArrowPressed();
        break;
    case 39:
        rightArrowPressed();
        break;
    case 40:
        downArrowPressed();
        break;
    }
};