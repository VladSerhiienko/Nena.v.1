namespace UniversityCodeFirst.Migrations
{
    using System.Data.Entity.Migrations;

    public partial class InitialCreate : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.Disciplines",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        Title = c.String(nullable: false, maxLength: 50),
                    })
                .PrimaryKey(t => t.Id);

            CreateTable(
                "dbo.Groups",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        Acronym = c.String(nullable: false, maxLength: 5),
                        Course = c.Int(nullable: false),
                        DepartmentIndex = c.Int(nullable: false),
                    })
                .PrimaryKey(t => t.Id);

            CreateTable(
                "dbo.Students",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        Name = c.String(nullable: false, maxLength: 20),
                        Surname = c.String(nullable: false, maxLength: 20),
                        Group_Id = c.Int(),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("dbo.Groups", t => t.Group_Id)
                .Index(t => t.Group_Id);

            CreateTable(
                "dbo.Disciplines_Groups",
                c => new
                    {
                        Discipline_Id = c.Int(nullable: false),
                        Group_Id = c.Int(nullable: false),
                    })
                .PrimaryKey(t => new { t.Discipline_Id, t.Group_Id })
                .ForeignKey("dbo.Disciplines", t => t.Discipline_Id, cascadeDelete: true)
                .ForeignKey("dbo.Groups", t => t.Group_Id, cascadeDelete: true)
                .Index(t => t.Discipline_Id)
                .Index(t => t.Group_Id);
        }

        public override void Down()
        {
            DropIndex("dbo.Disciplines_Groups", new[] { "Group_Id" });
            DropIndex("dbo.Disciplines_Groups", new[] { "Discipline_Id" });
            DropIndex("dbo.Students", new[] { "Group_Id" });
            DropForeignKey("dbo.Disciplines_Groups", "Group_Id", "dbo.Groups");
            DropForeignKey("dbo.Disciplines_Groups", "Discipline_Id", "dbo.Disciplines");
            DropForeignKey("dbo.Students", "Group_Id", "dbo.Groups");
            DropTable("dbo.Disciplines_Groups");
            DropTable("dbo.Students");
            DropTable("dbo.Groups");
            DropTable("dbo.Disciplines");
        }
    }
}