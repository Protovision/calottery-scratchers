class Application extends React.Component{
	constructor(properties){
		super(properties);
		this.state={
			statusFilter:"Any",
			typeFilter:"Any",
			priceFilter:"Any",
			scratchers:"None"};
		this.onStatusFilterChange=this.onStatusFilterChange.bind(this);
		this.onTypeFilterChange=this.onTypeFilterChange.bind(this);
		this.onPriceFilterChange=this.onPriceFilterChange.bind(this);
		this.onSubmit=this.onSubmit.bind(this)};
	render(){
		let view;
		if(this.state.scratchers=="None"){
			view=React.createElement(MaterialUI.Box)}
		else if(this.state.scratchers=="Pending"){
			view=React.createElement(
				MaterialUI.Box,{
					display:"flex",
					margin:"1em 0",
					justifyContent:"center"},
				React.createElement(MaterialUI.CircularProgress))}
		else{
			view=React.createElement(
				MaterialUI.Box,{
					margin:"1em 0"},
				React.createElement(
					MaterialUI.TableContainer,
					{},
					React.createElement(
						MaterialUI.Table,
						{},
						React.createElement(
							MaterialUI.TableHead,
							{},
							React.createElement(
								MaterialUI.TableRow,
								{},
								["Number","Name","Status","Type","Price ($)","Payout (%)"].map(
									(column)=>React.createElement(
										MaterialUI.TableCell,
										{key:column},
										column)))),
						React.createElement(
							MaterialUI.TableBody,
							{},
							this.state.scratchers.map(
								(row)=>React.createElement(
									MaterialUI.TableRow,
									{key:row.number},
									React.createElement(
										MaterialUI.TableCell,
										{},
										row.number),
									React.createElement(
										MaterialUI.TableCell,
										{},
										React.createElement(
											MaterialUI.Link,{
												href:row.productLink},
											row.name)),
									React.createElement(
										MaterialUI.TableCell,
										{},
										row.status),
									React.createElement(
										MaterialUI.TableCell,
										{},
										row.type),
									React.createElement(
										MaterialUI.TableCell,
										{},
										row.price),
									React.createElement(
										MaterialUI.TableCell,
										{},
										row.payout)))))))};
		return(
			React.createElement(
				MaterialUI.Box,{
					margin:"1em auto",
					boxSizing:"border-box",
					maxWidth:"64em"},
				React.createElement(
					MaterialUI.Typography,{
						variant:"h1",
						align:"center"},
					"CALottery Scratcher Insight"),
				React.createElement(
					MaterialUI.Box,{
						display:"flex",
						margin:"1em 0",
						justifyContent:"center",
						alignItems:"flex-end"},
					React.createElement(
						MaterialUI.Box,{
							margin:"0 0.5em"},
						React.createElement(
							MaterialUI.FormControl,
							{},
							React.createElement(
								MaterialUI.InputLabel,
								{},
								"Status:"),
							React.createElement(
								MaterialUI.Select,{
									value:this.state.statusFilter,
									onChange:this.onStatusFilterChange},
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Any"},
									"Any"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Newest Games"},
									"Newest Games"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Hot Sellers"},
									"Hot Sellers"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Games Ending Soon"},
									"Games Ending Soon")))),
					React.createElement(
						MaterialUI.Box,{
							margin:"0 0.5em"},
						React.createElement(
							MaterialUI.FormControl,
							{},
							React.createElement(
								MaterialUI.InputLabel,
								{},
								"Type:"),
							React.createElement(
								MaterialUI.Select,{
									value:this.state.typeFilter,
									onChange:this.onTypeFilterChange},
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Any"},
									"Any"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Crossword"},
									"Crossword"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Poker"},
									"Poker"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Bingo"},
									"Bingo"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Tic-Tac-Toe"},
									"Tic-Tac-Toe"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Match 3"},
									"Match 3"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Key Number Match"},
									"Key Number Match"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Key Number Match and Match 3"},
									"Key Number Match and Match 3"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Find Legend"},
									"Find Legend"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Match Up"},
									"Match Up")))),
					React.createElement(
						MaterialUI.Box,{
							margin:"0 0.5em"},
						React.createElement(
							MaterialUI.FormControl,
							{},
							React.createElement(
								MaterialUI.InputLabel,
								{},
								"Price:"),
							React.createElement(
								MaterialUI.Select,{
									value:this.state.priceFilter,
									onChange:this.onPriceFilterChange},
								React.createElement(
									MaterialUI.MenuItem,{
										value:"Any"},
									"Any"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"1"},
									"$1"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"2"},
									"$2"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"3"},
									"$3"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"5"},
									"$5"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"10"},
									"$10"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"20"},
									"$20"),
								React.createElement(
									MaterialUI.MenuItem,{
										value:"30"},
									"$30")))),
					React.createElement(
						MaterialUI.Box,{
							margin:"0 0.5em"},
						React.createElement(
							MaterialUI.Button,{
								variant:"contained",
								onClick:this.onSubmit},
							"Submit"))),
				view))}
	onStatusFilterChange(event){
		this.setState({statusFilter:event.target.value})};
	onTypeFilterChange(event){
		this.setState({typeFilter:event.target.value})}
	onPriceFilterChange(event){
		this.setState({priceFilter:event.target.value})};
	async onSubmit(){
		console.log(this.state);
		this.setState({scratchers:"Pending"});
		const resource=
			"/api?status="+encodeURIComponent(this.state.statusFilter)+"&"+
			"type="+encodeURIComponent(this.state.typeFilter)+"&"+
			"price="+encodeURIComponent(this.state.priceFilter);
		try{
			this.setState({scratchers:await ((await fetch(resource)).json())})}
		catch(exception){
			console.log(exception)}}};
ReactDOM.render(
	React.createElement(Application),
	document.querySelector("main"))
