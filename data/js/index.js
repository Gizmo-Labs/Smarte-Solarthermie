const path = window.location.pathname;
console.log(path);

; (function () {
  function id(v) { return document.getElementById(v); }
  function loadbar() {
    var ovrl = id("overlay"),
      prog = id("progress"),
      stat = id("progstat"),
      img = document.images,
      c = 0,
      tot = 5000;
    if (tot == 0) return doneLoading();

    function doneLoading() {
      ovrl.style.opacity = 0;
      setTimeout(function () {
        ovrl.style.display = "none";
      }, 1200);
    }
    var ready = false;
    $(document).ready(function () {
      ready = true;
    });
    while (ready == false) {
      for (var i = 0; i <= tot; i++) {
        var perc = ((100 / tot * i) << 0) + "%";
        prog.style.width = perc;
        stat.innerHTML = "Loading " + perc;
        if (i === tot) return doneLoading();
      }
    }
  }
  document.addEventListener('DOMContentLoaded', loadbar, false);
}());

function letterTransition() {
  if (path == '/index.html' || path == '/') 
  {
    // Wrap every letter in a span
    var textWrapper = document.querySelector('.letters');
    textWrapper.innerHTML = textWrapper.textContent.replace(/\S/g, "<span class='letter'>$&</span>");

    anime.timeline({ loop: true })
      .add({
        targets: '.letters .letter',
        opacity: [0, 0.6],
        easing: "easeInOutQuad",
        duration: 1000,
        delay: (el, i) => 250 * (i + 1)
      })
      .add({
        targets: '.letters',
        opacity: 0,
        duration: 1000,
        easing: "easeOutExpo",
        delay: 10000
      });

    var tl = gsap.timeline();
    tl.from('.letters', { duration: 3, translateY: 80, opacity: 1, ease: 'power1.inOut' })
    tl.to('#neon a', { duration: 1, opacity: 0.7 });
  }
}

function heart() {  
  if (path == '/index.htm' || path == '/admin.htm' || path == '/') 
  {
    anime({
      targets: '.love',
      opacity: [0, 0.6],
      translateX: ['300%', '0%'],
      duration: 5000,
      easing: "easeInOutQuad",
    });
  }
}


$(document).ready(function() {
  letterTransition();
  heart();
});

