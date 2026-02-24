const jwt = require('jsonwebtoken');

function authenticateToken(req, res, next) {
    const authHeader = req.headers['authorization'];
    const token = authHeader && authHeader.split(' ')[1];

    //Check if token exists
    if (token == null) return res.status(401).json({ error: "Token missing" });

    //Verify token validity
    jwt.verify(token, process.env.MY_TOKEN, function(err, user) {
        if (err) return res.status(403).json({ error: "Invalid token" });

        //Store decoded info for authorization
        req.user = user;
        next();
    });
}

module.exports = authenticateToken;