window.addEventListener('load', () => {
    renew(), once(), solar(), smart();
    interval = setInterval(renew, 1000);
    solar = setInterval(solar, 1000);
    smart = setInterval(smart, 4000);
    let button = document.querySelectorAll('button');
    let span = document.querySelectorAll('#right span');

    button[1].addEventListener('click', () => {
        if (confirm('Dies verbindet in 5 Sekunden das Gerät neu! Bist du sicher!'));
        {
            re('reconnect');
        }
    });

    button[2].addEventListener('click', () => {
        if (confirm('Dies startet in 5 Sekunden das Gerät neu! Bist du sicher!'));
        {
            re('restart');
        }
    });

    function once(arg1) {
        console.log("Once");
        fetch('/status/once').then((resp) => {
            return resp.json();
        }).then((obj) => {
            if (arg1 === undefined) {
                interval = setInterval(renew, 1000);
                solar = setInterval(solar, 1000);
                smart = setInterval(smart, 4000);
            }
            span[4].innerHTML = obj['LocalIP'];
            span[5].innerHTML = obj['Hostname'];
            span[6].innerHTML = obj['SSID'];
            span[7].innerHTML = obj['GatewayIP'];
            span[8].innerHTML = obj['Channel'];
            span[9].innerHTML = obj['MacAddress'];
            span[10].innerHTML = obj['SubnetMask'];
            span[11].innerHTML = obj['BSSID'];
            span[12].innerHTML = obj['ClientIP'];
            span[13].innerHTML = obj['DnsIP'];
        }).catch(function (err) {
            console.log("Error");
            re();
        });
    }

    function renew() {
        fetch('status/renew').then((resp) => {
            return resp.json();
        }).then((array) => {
            array.forEach((x, i) => {
                span[i].innerHTML = x
            });
        });
        console.log("renew");
    }

    function solar() {
        let Betriebsart = document.getElementById("Betriebsart");
        let KollOst = document.getElementById("KollOst");
        let KollWest = document.getElementById("KollWest");
        let Warmwasser = document.getElementById("Warmwasser");
        let PuUnten = document.getElementById("PuUnten");
        let PuMitte = document.getElementById("PuMitte");
        let Vorlauf = document.getElementById("Vorlauf");
        let Ruecklauf = document.getElementById("Ruecklauf");
        let SolarpumpeOst = document.getElementById("SolarpumpeOst");
        let SolarpumpeWest = document.getElementById("SolarpumpeWest");
        let Ladepumpe = document.getElementById("Ladepumpe");
        let Umschaltventil = document.getElementById("Umschaltventil");
        fetch('status/solar').then((resp) => {
            return resp.json();
        }).then((array) => {
            array.forEach((x, i) => {
                switch (array[0]) {
                    case "0":
                        Betriebsart.innerHTML = "Handbetrieb";
                        break;

                    case "1":
                        Betriebsart.innerHTML = "Automatikbetrieb";
                        break;
                }

                KollOst.innerHTML = array[1]
                KollWest.innerHTML = array[2]
                Warmwasser.innerHTML = array[3]
                PuMitte.innerHTML = array[4]
                PuUnten.innerHTML = array[5]
                Vorlauf.innerHTML = array[6]
                Ruecklauf.innerHTML = array[7]

                switch (array[8]) {
                    case "0":
                        SolarpumpeOst.innerHTML = "Inaktiv";
                        break;

                    case "1":
                        SolarpumpeOst.innerHTML = "Aktiv";
                        break;
                }

                switch (array[9]) {
                    case "0":
                        SolarpumpeWest.innerHTML = "Inaktiv";
                        break;

                    case "1":
                        SolarpumpeWest.innerHTML = "Aktiv";
                        break;
                }

                switch (array[10]) {
                    case "0":
                        Ladepumpe.innerHTML = "Inaktiv";
                        break;

                    case "1":
                        Ladepumpe.innerHTML = "Aktiv";
                        break;
                }

                switch (array[11]) {
                    case "0":
                        Umschaltventil.innerHTML = "Inaktiv";
                        break;

                    case "1":
                        Umschaltventil.innerHTML = "Aktiv";
                        break;
                }
            });
        });
    }


    function smart() {
        let Spannung = document.getElementById("Spannung");
        let Strom = document.getElementById("Strom");
        let Leistung = document.getElementById("Leistung");
        let Arbeit = document.getElementById("Arbeit");
        let Waerme = document.getElementById("Waerme");

        fetch('status/smart').then((resp) => {
            return resp.json();
        }).then((array) => {
            array.forEach((x, i) => {
                Spannung.innerHTML = array[0]
                Strom.innerHTML = array[1]
                Leistung.innerHTML = array[2]
                Arbeit.innerHTML = array[3]
                Waerme.innerHTML = array[4]
            });
        });
    }

    function re(arg) {
        clearInterval(solar);
        clearInterval(smart);
        clearInterval(interval);
        fetch(arg);
        if (arg == 'restart') {
            setTimeout(once, 5000);
        }
        else if (arg == 'reconnect') {
            setTimeout(once, 5000);
        }
        else {
            setTimeout(once, 1000);
        }
    }
});

