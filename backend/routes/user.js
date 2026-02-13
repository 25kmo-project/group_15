//ladataan express-framework, jotta voidaan luoda web-palvelin
//tässä express on muuttuja ja sinne ladataan express-kirjasto(meillä on jo asennettu: npm install express)
const express = require('express');
//käsittelee url-polkuja
const router = express.Router();
//haetaan tiedosto, joka sisältää tietokannan funktioita
const user = require('../models/user_model');

//haetaan kaikki userit
router.get('/', function (request, response){
    user.getAll(function(err, result){
        if(err){
            response.send (err);
        }
        else{
            response.json (result);
        }
    })
});

//haetaat yksi user
router.get('/:user_id', function(request, response){
    user.getOne(request.params.user_id, function(err,result){
        if(err){
            response.send(err);
        }
        else{
            response.json(result[0]);
        }
    })
});

//lisätään uusi user
router.post('/:user_id',function(request,response){
    user.add(request.body,request.params.user_id,function(err,result){
        if(err){
            response.send(err);
        }
        else{
            response.json(result);
        }
    })
});

//päivittää user:n tietoja
router.put('/:user_id', function(request, response){
    user.update(request.body, request.params.user_id, function(err, result){
        if(err){
            response.send(err);
        }
        else{
            //tarvitaanko rivi console.log??????
            console.log(result.affectedRows);
            response.json(result);
        }
    });
});

//poistaa kokonaan user tietokannasta
router.delete('/:user_id', function(request, response){
    user.delete(request.params.user_id, function(err, result){
        if(err){
            response.send(err);
        }
        else{
            response.json(result);
        }
    });
});

//tämä antaa mahdollisuus käyttää tätä tiedostoa muissa tiedostoissa
module.exports=router;